#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <string.h>
#include <stdarg.h>
#include <stdint.h>
#include <sys/ioctl.h>

// Base address and size definitions
#define ISP_BASE_ADDR        0x13300000
#define ISP_MAP_SIZE         0x100000
#define ISP_OFFSET_PARAMS    0x137f0
#define NUM_ENTRIES          471

// CPU info register addresses from decompiled code
#define CPU_ID_ADDR      0x1300002c    // SOC ID register
#define CPPSR_ADDR       0x10000034    // CPPSR register
#define SUBTYPE_ADDR     0x13540238    // Subsystem type register
#define SUBREMARK_ADDR   0x13540231    // Subremark register

// Register offsets from binary analysis
#define REG_CONTROL          0x4
#define REG_BAYER_PATTERN    0x8
#define REG_BYPASS           0xc
#define REG_DEIR_CONTROL     0x1c
#define REG_STATUS           0x30
#define REG_BUFFER_BASE      0xa02c
#define REG_BUFFER_CONFIG    0xa04c

// Global buffers at fixed addresses
#define BUF_A2F60    0xa2f60
#define BUF_A2E48    0xa2e48
#define BUF_A2E14    0xa2e14
#define BUF_A2DE0    0xa2de0
#define BUF_A2DAC    0xa2dac

struct tisp_param_info {
    uint8_t data[64];  // Adjust the size as per actual structure usage
};

// Updated ISP device structure based on binary ninja offsets
typedef struct {
    char dev_name[32];     // Device name/path
    int fd;                // File descriptor (offset 0x20)
    int is_open;           // Device open status (offset 0x24)
    char sensor_name[80];  // Sensor name buffer (offset 0x28)
    char padding1[0x60];   // Padding to 0xac
    void* buf_info;        // Buffer info pointer (offset 0xac)
    int wdr_mode;          // WDR mode flag (offset 0xb0)
    void* wdr_buf_info;    // WDR buffer info pointer (offset 0xb4)
    char padding2[0x28];   // Padding to match 0xE0 size
} IMPISPDev;

static IMPISPDev* gISPdev = NULL;

typedef struct {
    char type[32];
    int addr;
    // Add any other i2c fields needed
} imp_isp_i2c_info;

typedef struct {
    char name[32];
    int cbus_type;
    imp_isp_i2c_info i2c;
    // Add other sensor fields as needed
} IMPSensorInfo;


typedef int (*alloc_func_t)(int size);
typedef int (*get_paddr_func_t)(int result);

typedef struct {
    void* (*alloc_mem)(size_t);
    int (*sp_alloc_mem)(void**, int); // Adjusted to match spAllocMem
    void (*free_mem)(void**, int);
    void** (*get_alloc_info)(void**, int);
    int (*get_mem_attr)(void**, int);
    int (*set_mem_attr)(void**, int, int);
    int (*dump_mem_status)(void**);
    int (*dump_mem_to_file)(int*);
    alloc_func_t alloc;
    get_paddr_func_t get_paddr;
} AllocFuncs;

// Static variables matching the decompiled structure
static uint32_t soc_id = 0xFFFFFFFF;      // .8648
static uint32_t cppsr = 0xFFFFFFFF;       // .8647
static uint32_t subsoctype = 0xFFFFFFFF;  // .8649
static uint32_t subremark = 0xFFFFFFFF;   // .8651
static int g_devmem_fd = -1;


// Global variables from decompiled code
void *map_base = MAP_FAILED;
int mem_fd;
uint8_t *tparams_day = NULL, *tparams_night = NULL, *tparams_cust = NULL;
int isp_memopt = 1;
int deir_en = 0;
uint32_t data_a2f64 = 0;
uint32_t data_a2ea4 = 0;
static int data_108f38 = 0;
char data_108f3c[32];
char data_108f78[32];
static int (*alloc_func)(int, int, int) = NULL;

// Function prototypes
static void write_isp_register(uint32_t offset, uint32_t value);
static uint32_t read_isp_register(uint32_t offset);
static int init_buffer_config(void);
static void init_bayer_pattern(uint32_t pattern);
static int verify_register_write(uint32_t offset, uint32_t value);
int regrw(int32_t offset, int32_t *value, int32_t write);
static int32_t get_cpuinfo(int32_t offset, int32_t *value);
static int32_t get_cpu_id(void);
static void read_soc_info();
int tisp_init(struct tisp_param_info *param, const char *bin_path);
void cleanup();


// Helper function declarations
static void *private_vmalloc(size_t size) {
    void *ptr = malloc(size);  // For testing, replace with actual vmalloc
    printf("Allocated %zu bytes at %p\n", size, ptr);
    return ptr;
}

static void system_reg_write(uint32_t offset, uint32_t value) {
    if (!map_base) {
        printf("Error: Memory not mapped\n");
        return;
    }
    volatile uint32_t *reg = (uint32_t *)((char *)map_base + offset);
    *reg = value;
    printf("Write: reg[0x%x] = 0x%08x\n", offset, value);
}

static int tiziano_load_parameters(const char *filename) {
    FILE *f = fopen(filename, "rb");
    if (!f) {
        printf("Failed to open parameter file: %s\n", filename);
        return -1;
    }
    // TODO: Implement actual parameter loading
    fclose(f);
    return 0;
}

// Log level definitions (matching binary ninja's imp_log_fun levels)
#define IMP_LOG_ERROR   6
#define IMP_LOG_INFO    3
#define IMP_LOG_DEBUG   4

// Mock implementation of the logging function seen in binary ninja
static void imp_log_fun(int level, int option, int arg2, const char* file,
                       const char* func, int line, const char* fmt, ...) {
    // Simple implementation for testing
    printf("LOG [%d]: ", level);
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
    printf("\n");
}

// Mock implementation of IMP_Log_Get_Option
static int IMP_Log_Get_Option(void) {
    return 0;
}

/* Function declarations that would be in IMP */
static int IMP_ISP_Open(void);
static int IMP_ISP_AddSensor(char* sensor_name);
static int IMP_ISP_EnableSensor(void);
static int IMP_System_Init(void);
static int IMP_ISP_EnableTuning(void);
static int IMP_Alloc(void *arg1, int size, int type);
int IMP_init();
extern int IMP_Get_Info(void* arg1, int addr);

#define ISP_BUFFER_TYPE 1  // Define a type code

// Function declarations
void* simpleAlloc(size_t size);
int allocWithManager(AllocFuncs *manager, int size, const char *name);
int spAllocMem(void** arg1, int arg2);
void freeMem(void** arg1, int arg2);
void** alloc_get_info(void** arg1, int arg2);
int getMemAttr(void** arg1, int arg2);
int setMemAttr(void** arg1, int arg2, int arg3);
int dumpMemStatus(void** arg1);
int dumpMemToFile(int* arg1);
int alloc_dump_to_file(int* arg1);


// Global variable declaration
AllocFuncs g_alloc;

int spAllocMem(void** arg1, int arg2) {
    int log_option;
    int var_1c;
    int result = 0;

    // Check if the argument is NULL
    if (arg1 == NULL) {
        log_option = IMP_Log_Get_Option();
        imp_log_fun(6, log_option, 2, __FILE__, __func__, __LINE__, "Alloc Manager", "%s function param is NULL\n", "spAllocMem");
        return 0;
    }

    // Iterate through the linked list
    while (arg1 != NULL) {
        // Check if arg2 matches arg1[9]
        if (arg2 == ((int*)arg1)[9]) {
            ((int*)arg1)[12] += 1; // Increment arg1[0xc]
            return arg2;
        }
        // Move to the next element in the linked list
        arg1 = (void**)*arg1;
    }

    // If not found, log an error
    log_option = IMP_Log_Get_Option();
    imp_log_fun(6, log_option, 2, __FILE__, __func__, __LINE__, "Alloc Manager", "%s alloc get info failed\n", "spAllocMem");

    return 0;
}

#include <stdio.h>
#include <stdlib.h>

// Function to retrieve information from a linked list
void** alloc_get_info(void** arg1, int arg2) {
    // Set the initial result to the input argument
    void** result = arg1;

    // Check if the input argument is NULL
    if (arg1 == NULL) {
        return result;
    }

    // Check if the value at index 9 matches arg2
    if (arg2 == ((int*)arg1)[9]) {
        return result;
    }

    // Traverse the linked list
    while (1) {
        result = *result; // Move to the next node

        // If the end of the list is reached, return NULL
        if (result == NULL) {
            return result;
        }

        // Check if the value at index 9 matches arg2
        if (arg2 == ((int*)result)[9]) {
            return result;
        }
    }
}

void* simpleAlloc(size_t size) {
    return malloc(size);
}

int allocWithManager(AllocFuncs *manager, int size, const char *name) {
    if (manager == NULL || manager->alloc_mem == NULL) {
        return -1;
    }
    return (int)manager->alloc_mem(size);
}

int allocInit(AllocFuncs *alloc) {
    if (alloc != NULL) {
        alloc->alloc_mem = simpleAlloc;
        alloc->sp_alloc_mem = spAllocMem;
        alloc->free_mem = freeMem;
        alloc->get_alloc_info = alloc_get_info;
        alloc->get_mem_attr = getMemAttr;
        alloc->set_mem_attr = setMemAttr;
        alloc->dump_mem_status = dumpMemStatus;
        alloc->dump_mem_to_file = dumpMemToFile;
        return 0;
    }
    return -1;
}

void freeMem(void** arg1, int arg2) {
    int log_option;

    // Check if the input argument is NULL
    if (arg1 == NULL) {
        log_option = IMP_Log_Get_Option();
        imp_log_fun(6, log_option, 2, __FILE__, __func__, __LINE__, "Alloc Manager", "%s function param is NULL\n", "freeMem");
        return;
    }

    // Traverse the linked list
    void** current = arg1;
    while (current != NULL) {
        int j = ((int*)current)[9];

        // If arg2 matches the value at index 9
        if (arg2 == j) {
            // Decrement the counter at index 12
            int count = ((int*)current)[12] - 1;
            ((int*)current)[12] = count;

            // If the count reaches zero, free the memory
            if (count == 0) {
                void (*free_func)(int) = (void (*)(int))current[10];
                if (free_func != NULL) {
                    free_func(j);
                }

                // Unlink and free the node
                void** next_node = *current;
                if (next_node == NULL) {
                    free(arg1);
                    return;
                } else {
                    *arg1 = next_node;
                    free(current);
                    return;
                }
            }
            return;
        }
        current = (void**)*current;
    }

    // Log an error if the function fails to find the node
    log_option = IMP_Log_Get_Option();
    imp_log_fun(6, log_option, 2, __FILE__, __func__, __LINE__, "Alloc Manager", "%s alloc get info failed\n", "freeMem");

    return;
}

int getMemAttr(void** arg1, int arg2) {
    int log_option;

    // Check if the input argument is NULL
    if (arg1 == NULL) {
        log_option = IMP_Log_Get_Option();
        imp_log_fun(6, log_option, 2, __FILE__, __func__, __LINE__, "%s function param is NULL\n", "getMemAttr");
        return 0xffffffff;
    }

    // Traverse the linked list
    while (arg1 != NULL) {
        // Check if arg2 matches the value at index 9
        if (arg2 == ((int*)arg1)[9]) {
            return ((int*)arg1)[13]; // Return the value at index 13 (0xd)
        }

        // Move to the next node in the linked list
        arg1 = (void**)*arg1;
    }

    // Log an error if the attribute is not found
    log_option = IMP_Log_Get_Option();
    imp_log_fun(6, log_option, 2, __FILE__, __func__, __LINE__, "%s alloc get info failed\n", "getMemAttr");

    return 0xffffffff; // Return an error code if not found
}

int setMemAttr(void** arg1, int arg2, int arg3) {
    int log_option;

    // Check if the input argument is NULL
    if (arg1 == NULL) {
        log_option = IMP_Log_Get_Option();
        imp_log_fun(6, log_option, 2, __FILE__, __func__, __LINE__, "%s function param is NULL\n", "setMemAttr");
        return 0xffffffff;
    }

    // Traverse the linked list
    while (arg1 != NULL) {
        // Check if arg2 matches the value at index 9
        if (arg2 == ((int*)arg1)[9]) {
            ((int*)arg1)[13] = arg3; // Set the value at index 13 (0xd) to arg3
            return 0; // Success
        }

        // Move to the next node in the linked list
        arg1 = (void**)*arg1;
    }

    // Log an error if the attribute is not found
    log_option = IMP_Log_Get_Option();
    imp_log_fun(6, log_option, 2, __FILE__, __func__, __LINE__, "%s alloc get info failed\n", "setMemAttr");

    return 0xffffffff; // Return an error code if not found
}

int dumpMemStatus(void** arg1) {
    int log_option;

    // Check if the input argument is NULL
    if (arg1 == NULL) {
        log_option = IMP_Log_Get_Option();
        imp_log_fun(6, log_option, 2, __FILE__, __func__, __LINE__, "%s function param is NULL\n", "dumpMemStatus");
        return 0xffffffff;
    }

    // Call the function at arg1[41] if it exists
    void (*custom_dump_func)() = (void (*)())arg1[41];
    if (custom_dump_func != NULL) {
        custom_dump_func();
    }

    // Traverse the linked list and log information
    void** i = arg1;
    while (i != NULL) {
        log_option = IMP_Log_Get_Option();

        // Log various attributes of the current node
        imp_log_fun(4, log_option, 2, __FILE__, __func__, __LINE__, "\ninfo->n_info = %p\n", i);
        imp_log_fun(4, log_option, 2, __FILE__, __func__, __LINE__, "info->owner = %s\n", (char*)i[1]);
        imp_log_fun(4, log_option, 2, __FILE__, __func__, __LINE__, "info->vaddr = 0x%08x\n", i[9]);
        imp_log_fun(4, log_option, 2, __FILE__, __func__, __LINE__, "info->paddr = 0x%08x\n", i[10]);
        imp_log_fun(4, log_option, 2, __FILE__, __func__, __LINE__, "info->length = %d\n", i[11]);
        imp_log_fun(4, log_option, 2, __FILE__, __func__, __LINE__, "info->ref_cnt = %d\n", i[12]);
        imp_log_fun(4, log_option, 2, __FILE__, __func__, __LINE__, "info->mem_attr = %d\n", i[13]);

        // Move to the next node in the list
        i = (void**)*i;
    }

    return 0;
}

int alloc_dump_to_file(int* arg1) {
    // Open the file for writing
    FILE *stream = fopen("/tmp/alloc_manager_info", "w");
    if (stream == NULL) {
        int log_option = IMP_Log_Get_Option();
        imp_log_fun(6, log_option, 2, __FILE__, "alloc_dump_to_file", __LINE__, "Failed to open /tmp/alloc_manager_info\n");
        return 0xffffffff;
    }

    // Traverse the linked list if arg1 is not NULL
    int* i = arg1;
    while (i != NULL) {
        char buffer[512]; // Buffer to hold the formatted string
        memset(buffer, 0, sizeof(buffer)); // Clear the buffer

        // Format the information into the buffer
        int len = 0;
        len += sprintf(buffer + len, "\ninfo->n_info = %p\n", i);
        len += sprintf(buffer + len, "info->owner = %s\n", (char*)i[1]);
        len += sprintf(buffer + len, "info->vaddr = 0x%08x\n", i[9]);
        len += sprintf(buffer + len, "info->paddr = 0x%08x\n", i[10]);
        len += sprintf(buffer + len, "info->length = %d\n", i[11]);
        len += sprintf(buffer + len, "info->ref_cnt = %d\n", i[12]);
        len += sprintf(buffer + len, "info->mem_attr = %d\n", i[13]);

        // Write the buffer to the file
        fwrite(buffer, len, 1, stream);

        // Move to the next node
        i = (int*)*i;
    }

    // Close the file
    fclose(stream);
    return 0;
}

int dumpMemToFile(int* arg1) {
    int log_option;

    // Check if the input argument is NULL
    if (arg1 == NULL) {
        log_option = IMP_Log_Get_Option();
        imp_log_fun(6, log_option, 2, __FILE__, "dumpMemToFile", __LINE__, "%s function param is NULL\n", "dumpMemToFile");
        return 0xffffffff;
    }

    // Call the function at arg1[42] if it exists
    void (*custom_dump_func)() = (void (*)())arg1[42];
    if (custom_dump_func != NULL) {
        custom_dump_func();
    }

    // Call alloc_dump_to_file to handle the actual dumping process
    alloc_dump_to_file(arg1);

    return 0;
}
#define SOC_ID_OFFSET     0x00  // Hypothetical offset for SOC ID
#define FAMILY_OFFSET     0x04  // Hypothetical offset for Family/Version
#define CPPSR_OFFSET      0x08  // Hypothetical offset for CPPSR

int regrw(int32_t offset, int32_t *value, int32_t write) {
    int fd;
    int32_t result = -1;
    int page_size = getpagesize();

    // Open /dev/mem with the appropriate access mode
    int access_mode = (write == 0) ? O_RDONLY : O_RDWR;
    fd = open("/dev/mem", access_mode);
    if (fd < 0) {
        perror("open /dev/mem failed");
        return -1;
    }

    // Align the offset to the page size
    int32_t aligned_offset = offset & ~(page_size - 1);
    int32_t page_offset = offset & (page_size - 1);

    // Map the memory region
    void *mapped_base = mmap(NULL, page_size, (write ? PROT_READ | PROT_WRITE : PROT_READ), MAP_SHARED, fd, aligned_offset);
    if (mapped_base == MAP_FAILED) {
        perror("mmap failed");
        close(fd);
        return -1;
    }

    // Perform the read or write operation
    volatile int32_t *reg_addr = (volatile int32_t *)((char *)mapped_base + page_offset);
    if (write == 0) {
        *value = *reg_addr;
    } else {
        *reg_addr = *value;
    }

    // Clean up
    munmap(mapped_base, page_size);
    close(fd);
    return 0;
}

void read_soc_info() {
    int32_t value;

    // Read SOC ID
    if (regrw(SOC_ID_OFFSET, &value, 0) == 0) {
        printf("SOC ID: 0x%08x\n", value);
    }

    // Read Family/Version
    if (regrw(FAMILY_OFFSET, &value, 0) == 0) {
        printf("Family: 0x%08x\n", value);
    }

    // Read CPPSR
    if (regrw(CPPSR_OFFSET, &value, 0) == 0) {
        printf("CPPSR: 0x%08x\n", value);
    }
}

int32_t get_cpuinfo(int32_t offset, int32_t *value) {
    if (value == NULL) {
        printf("Error: NULL pointer provided for value\n");
        return -1;
    }

    // Call regrw with arg3 = 0 for reading
    int result = regrw(offset, value, 0);

    // Debug output
    if (result == 0) {
        printf("Read CPU info: offset 0x%08x = 0x%08x\n", offset, *value);
    } else {
        printf("Error reading CPU info at offset 0x%08x\n", offset);
    }

    return result;
}

// Modified CPU detection matching decompiled logic
static int32_t get_cpu_id(void) {
    int32_t result;

    // Step 1: SOC ID check
    if (soc_id == 0xFFFFFFFF) {
        result = get_cpuinfo(CPU_ID_ADDR, &soc_id);
        if (result < 0 || soc_id == 0xFFFFFFFF) {
            imp_log_fun(IMP_LOG_ERROR, IMP_Log_Get_Option(), 2,
                       __FILE__, __func__, __LINE__,
                       "get_cpuinfo id reg 0x%x error\n", CPU_ID_ADDR);
            return -1;
        }
    }

    // Step 2: CPPSR check
    if (cppsr == 0xFFFFFFFF) {
        result = get_cpuinfo(CPPSR_ADDR, &cppsr);
        if (result < 0 || cppsr == 0xFFFFFFFF) {
            imp_log_fun(IMP_LOG_ERROR, IMP_Log_Get_Option(), 2,
                       __FILE__, __func__, __LINE__,
                       "get_cpuinfo cppsr reg 0x%x error\n", CPPSR_ADDR);
            return -1;
        }
    }

    // Step 3: Subtype check
    if (subsoctype == 0xFFFFFFFF) {
        result = get_cpuinfo(SUBTYPE_ADDR, &subsoctype);
        if (result < 0 || subsoctype == 0xFFFFFFFF) {
            imp_log_fun(IMP_LOG_ERROR, IMP_Log_Get_Option(), 2,
                       __FILE__, __func__, __LINE__,
                       "get_cpuinfo subtype reg 0x%x error\n", SUBTYPE_ADDR);
            return -1;
        }
    }

    // Step 4: Subremark check
    if (subremark == 0xFFFFFFFF) {
        result = get_cpuinfo(SUBREMARK_ADDR, &subremark);
        if (result < 0 || subremark == 0xFFFFFFFF) {
            imp_log_fun(IMP_LOG_ERROR, IMP_Log_Get_Option(), 2,
                       __FILE__, __func__, __LINE__,
                       "get_cpuinfo subremark reg 0x%x error\n", SUBREMARK_ADDR);
            return -1;
        }
    }

    // Process SOC ID fields
    uint32_t soc_family = (soc_id >> 28);     // Top 4 bits
    uint32_t soc_type = (soc_id >> 12) & 0xF; // Bits [15:12]

    printf("CPU Detection Results:\n");
    printf("  SOC ID:     0x%08x (Family: %d, Type: %d)\n", soc_id, soc_family, soc_type);
    printf("  CPPSR:      0x%08x\n", cppsr);
    printf("  Subtype:    0x%08x\n", subsoctype);
    printf("  Subremark:  0x%08x\n", subremark);

    // SOC family must be 1
    if (soc_family != 1) {
        return -1;
    }

    // Determine exact chip type based on decompiled logic
    // This is a simplified version - we can expand based on your needs
    switch (soc_type) {
        case 0x5:  // Original T10/T20
            if ((cppsr & 0xFF) == 1) return 0;
            if ((cppsr & 0xFF) == 0x10) return 2;
            break;

        case 0x30: // T30/T31
            if ((cppsr & 0xFF) == 1) {
                if ((subsoctype & 0xFFFF) == 0x3333) return 7;
                if ((subsoctype & 0xFFFF) == 0x1111) return 7;
                if ((subsoctype & 0xFFFF) == 0x2222) return 8;
                if ((subsoctype & 0xFFFF) == 0x4444) return 9;
                if ((subsoctype & 0xFFFF) == 0x5555) return 10;
            }
            break;
    }

    return -1;
}

int IMP_Alloc(void *arg1, int size, int type) {
    if (arg1 == NULL) {
        int log_option = IMP_Log_Get_Option();
        imp_log_fun(6, log_option, 2, __FILE__, __func__, __LINE__,
                    "IMPAlloc *alloc = NULL\n");
        return -1;
    }

    // Check if already initialized, otherwise initialize
    if (data_108f38 != 1) {
        if (IMP_init() != 0) {
            int log_option = IMP_Log_Get_Option();
            imp_log_fun(6, log_option, 2, __FILE__, __func__, __LINE__,
                        "imp init failed\n");
            return -1;
        }
        data_108f38 = 1;
    }

    // Check if the allocation function is available
    if (alloc_func != NULL) {
        int allocated_addr = alloc_func(0x108f00, size, type);
        if (allocated_addr == 0) {
            int log_option = IMP_Log_Get_Option();
            imp_log_fun(6, log_option, 2, __FILE__, __func__, __LINE__,
                        "g_alloc.alloc_mem failed\n");
            return -1;
        }

        // Retrieve additional information about the allocated memory
        if (IMP_Get_Info(arg1, allocated_addr) != 0) {
            int log_option = IMP_Log_Get_Option();
            imp_log_fun(6, log_option, 2, __FILE__, __func__, __LINE__,
                        "IMP_Get_info failed\n");
            return -1;
        }

        return 0;
    }

    // Fallback if allocation function is not set
    int log_option = IMP_Log_Get_Option();
    imp_log_fun(6, log_option, 2, __FILE__, __func__, __LINE__,
                "No allocation function set\n");
    return -1;
}

int IMP_ISP_Open(void) {
    int result = 0;

    printf("IMP_ISP_Open called\n");

    // Check if device is already initialized
    if (gISPdev == 0) {
        gISPdev = (IMPISPDev*)calloc(1, sizeof(IMPISPDev));
        if (gISPdev == NULL) {
            imp_log_fun(IMP_LOG_ERROR, IMP_Log_Get_Option(), 2,
                        __FILE__, __func__, __LINE__,
                        "Failed to alloc gISPdev!\n");
            return -1;
        }

        strcpy(gISPdev->dev_name, "/dev/tx-isp");
        gISPdev->fd = open(gISPdev->dev_name, O_RDWR | O_NONBLOCK);
        if (gISPdev->fd < 0) {
            imp_log_fun(IMP_LOG_ERROR, IMP_Log_Get_Option(), 2,
                        __FILE__, __func__, __LINE__,
                        "Cannot open %s\n", gISPdev->dev_name);
            free(gISPdev);
            gISPdev = NULL;
            return -1;
        }

        gISPdev->is_open = 1;
        imp_log_fun(IMP_LOG_INFO, IMP_Log_Get_Option(), 2,
                    __FILE__, __func__, __LINE__,
                    "~~~~~~ %s[%d] ~~~~~~~\n", "IMP_ISP_Open", __LINE__);

        // Detect and log CPU ID
        if (get_cpu_id() < 0) {
            imp_log_fun(IMP_LOG_ERROR, IMP_Log_Get_Option(), 2,
                        __FILE__, __func__, __LINE__,
                        "Failed to detect CPU ID\n");
        }
    }

    return result;
}

int32_t IMP_ISP_AddSensor(char* sensor_name) {
    if (gISPdev == NULL) {
        // Log and return if the global device structure is uninitialized
        imp_log_fun(6, IMP_Log_Get_Option(), 2, __FILE__, __func__, __LINE__, "ISP device not initialized");
        return -1;
    }

    // Check if the device is open
    if (gISPdev->is_open < 2) {
        imp_log_fun(6, IMP_Log_Get_Option(), 2, __FILE__, __func__, __LINE__, "ISP device is not open");
        return -1;
    }

    // Register the sensor using ioctl
    if (ioctl(gISPdev->fd, 0x805056c1, sensor_name) != 0) {
        imp_log_fun(6, IMP_Log_Get_Option(), 2, __FILE__, __func__, __LINE__, "VIDIOC_REGISTER_SENSOR(%s) error", sensor_name);
        return -1;
    }

    // Check if there is an existing path and configure it if present
    if (gISPdev->buf_info != NULL) {
        int result = ioctl(gISPdev->fd, 0xc00456c7, gISPdev->buf_info);
        free(gISPdev->buf_info);
        gISPdev->buf_info = NULL;
        if (result != 0) {
            return result;
        }
    }

    // Search for the sensor by name
    int sensor_id = -1;
    int idx = 0;
    char sensor_buf[80] = {0};
    while (ioctl(gISPdev->fd, 0xc050561a, &idx) == 0) {
        if (strcmp(sensor_name, sensor_buf) == 0) {
            sensor_id = idx;
            break;
        }
        idx++;
    }

    // If the sensor was not found, log an error and return
    if (sensor_id == -1) {
        imp_log_fun(6, IMP_Log_Get_Option(), 2, __FILE__, __func__, __LINE__, "Sensor [%s] not found", sensor_name);
        return -1;
    }

    // Allocate buffer info and configure it
    void* buf_info = malloc(0x94);
    if (buf_info == NULL) {
        imp_log_fun(6, IMP_Log_Get_Option(), 2, __FILE__, __func__, __LINE__, "Memory allocation failed");
        return -1;
    }

    if (IMP_Alloc(buf_info, 0x94, ISP_BUFFER_TYPE) != 0) {
        free(buf_info);
        imp_log_fun(6, IMP_Log_Get_Option(), 2, __FILE__, __func__, __LINE__, "IMP_Alloc failed");
        return -1;
    }

    gISPdev->buf_info = buf_info;
    if (ioctl(gISPdev->fd, 0x800856d4, buf_info) != 0) {
        imp_log_fun(6, IMP_Log_Get_Option(), 2, __FILE__, __func__, __LINE__, "VIDIOC_SET_BUF_INFO error");
        free(buf_info);
        return -1;
    }

    // Additional configuration for Wide Dynamic Range (WDR) if needed
    if (gISPdev->wdr_mode) {
        void* wdr_buf_info = malloc(0x94);
        if (wdr_buf_info != NULL) {
            if (IMP_Alloc(wdr_buf_info, 0x94, ISP_BUFFER_TYPE) == 0) {
                gISPdev->wdr_buf_info = wdr_buf_info;
                if (ioctl(gISPdev->fd, 0x800856d6, wdr_buf_info) != 0) {
                    imp_log_fun(6, IMP_Log_Get_Option(), 2, __FILE__, __func__, __LINE__, "VIDIOC_SET_WDR_BUF_INFO error");
                    free(wdr_buf_info);
                    return -1;
                }
            } else {
                free(wdr_buf_info);
            }
        }
    }

    imp_log_fun(4, IMP_Log_Get_Option(), 2, __FILE__, __func__, __LINE__, "Sensor [%s] successfully added", sensor_name);
    return 0;
}

// Function to safely write to a memory-mapped ISP register
static void write_isp_register(uint32_t offset, uint32_t value) {
    if (map_base == NULL) {
        printf("Error: Memory not mapped\n");
        return;
    }
    volatile uint32_t *reg = (uint32_t *)((char *)map_base + offset);
    *reg = value;
    printf("Wrote 0x%08x to register offset 0x%08x\n", value, offset);
}

// Function to safely read from a memory-mapped ISP register
static uint32_t read_isp_register(uint32_t offset) {
    if (map_base == NULL) {
        printf("Error: Memory not mapped\n");
        return 0;
    }
    volatile uint32_t *reg = (uint32_t *)((char *)map_base + offset);
    uint32_t value = *reg;
    printf("Read 0x%08x from register offset 0x%08x\n", value, offset);
    return value;
}

// Verify register write by reading back
static int verify_register_write(uint32_t offset, uint32_t value) {
    write_isp_register(offset, value);
    uint32_t readback = read_isp_register(offset);
    
    if (readback != value) {
        printf("Register verification failed at offset 0x%x: wrote 0x%x, read 0x%x\n",
               offset, value, readback);
        return -1;
    }
    return 0;
}

// Initialize bayer pattern and DEIR settings
static void init_bayer_pattern(uint32_t pattern) {
    printf("\nInitializing Bayer pattern with value 0x%x\n", pattern);
    
    if (pattern >= 0x15) {
        printf("Error: Unsupported bayer pattern: 0x%x\n", pattern);
        return;
    }

    uint32_t reg_value;
    uint32_t deir_enabled = 0;

    if (pattern <= 3) {
        reg_value = pattern;
    } else if (pattern <= 0x13) {
        reg_value = pattern + 4;
        deir_enabled = 1;
    } else { // pattern == 0x14
        deir_enabled = 1;
        reg_value = 0;
    }

    verify_register_write(REG_BAYER_PATTERN, reg_value);
    
    // Configure DEIR control register
    uint32_t deir_reg_value = deir_enabled ? 0x10003f00 : 0x3f00;
    verify_register_write(REG_DEIR_CONTROL, deir_reg_value);
}

// Initialize buffer configuration
static int init_buffer_config(void) {
    printf("\nInitializing buffer configuration\n");
    
    // Read initial status
    uint32_t initial_status = read_isp_register(REG_STATUS);
    printf("Initial status register: 0x%08x\n", initial_status);

    // Allocate memory for buffers (this is just an example, adjust sizes as needed)
    uint32_t buffer_size = 0x6000;
    void *buffer = malloc(buffer_size);
    if (!buffer) {
        printf("Failed to allocate buffer memory\n");
        return -1;
    }

    // Calculate physical address (this is simplified and may need adjustment)
    uint32_t phys_addr = (uint32_t)(uintptr_t)buffer;
    printf("Buffer allocated at virtual address: %p\n", buffer);
    
    // Configure buffer registers with verification
    if (verify_register_write(REG_BUFFER_BASE, phys_addr) < 0) return -1;
    if (verify_register_write(REG_BUFFER_BASE + 0x4, phys_addr + 0x1000) < 0) return -1;
    if (verify_register_write(REG_BUFFER_BASE + 0x8, phys_addr + 0x2000) < 0) return -1;
    if (verify_register_write(REG_BUFFER_BASE + 0xc, phys_addr + 0x3000) < 0) return -1;
    if (verify_register_write(REG_BUFFER_BASE + 0x10, phys_addr + 0x4000) < 0) return -1;
    if (verify_register_write(REG_BUFFER_BASE + 0x14, phys_addr + 0x4800) < 0) return -1;
    if (verify_register_write(REG_BUFFER_BASE + 0x18, phys_addr + 0x5000) < 0) return -1;
    if (verify_register_write(REG_BUFFER_BASE + 0x1c, phys_addr + 0x5800) < 0) return -1;
    
    // Set buffer configuration
    if (verify_register_write(REG_BUFFER_CONFIG, 0x33) < 0) return -1;

    // Read final status
    uint32_t final_status = read_isp_register(REG_STATUS);
    printf("Final status register: 0x%08x\n", final_status);

    return 0;
}


// Initialize ISP parameters with pointer version
static void isp_init_params(void *params) {
    printf("\nInitializing ISP parameters...\n");
    if (!params) {
        printf("Error: NULL parameter pointer\n");
        return;
    }

    // Read initial status
    uint32_t initial_status = read_isp_register(REG_STATUS);
    printf("Initial status before parameter load: 0x%08x\n", initial_status);

    uint32_t *param_array = (uint32_t *)params;
    for (int i = 0; i < NUM_ENTRIES; i++) {
        verify_register_write(ISP_OFFSET_PARAMS + i * 4, param_array[i]);
    }

    // Read final status
    uint32_t final_status = read_isp_register(REG_STATUS);
    printf("Final status after parameter load: 0x%08x\n", final_status);
}


// Helper function to allocate and clear memory
static uint8_t *allocate_and_clear_buffer(size_t size, const char *name) {
    uint8_t *buffer = private_vmalloc(size);
    if (!buffer) {
        printf("Failed to allocate %s buffer\n", name);
        return NULL;
    }
    memset(buffer, 0, size);
    return buffer;
}

// Helper function to configure DEIR control
static void configure_deir_control() {
    uint32_t deir_reg_value = deir_en ? 0x10003f00 : 0x3f00;
    system_reg_write(REG_DEIR_CONTROL, deir_reg_value);
}

// Refactored tisp_init function
int tisp_init(struct tisp_param_info *param, const char *bin_path) {
    // Allocate and initialize day/night parameter buffers
    tparams_day = allocate_and_clear_buffer(ISP_OFFSET_PARAMS, "tparams_day");
    tparams_night = allocate_and_clear_buffer(ISP_OFFSET_PARAMS, "tparams_night");
    if (!tparams_day || !tparams_night) {
        return -1;
    }

    // Handle bin file path
    char default_path[64];
    if (strlen(bin_path) == 0) {
        snprintf(default_path, sizeof(default_path), "/etc/sensor/sc2336-t31.bin");
        bin_path = default_path;
    }

    // Load parameters from file
    int ret = tiziano_load_parameters(bin_path);
    if (ret != 0) {
        printf("No bin file on the system!\n");
    } else {
        memcpy((void *)0x84b50, (void *)tparams_day, ISP_OFFSET_PARAMS);
    }

    // Configure hardware registers
    uint32_t reg_val = (param->data[0] << 16) | param->data[1];
    system_reg_write(REG_CONTROL, reg_val);
    data_a2f64 = param->data[1];

    // Configure DEIR control
    configure_deir_control();

    return 0;
}


int IMP_init() {
    // Initialize global variables
    strcpy(data_108f3c, "kmalloc");
    strncpy(data_108f78, "continuous", 11);

    // Call allocInit with the global allocator structure
    int result = allocInit(&g_alloc);

    // Check if allocInit failed
    if (result != 0) {
        imp_log_fun(6, IMP_Log_Get_Option(), 2, __FILE__, __func__, __LINE__, "IMP Alloc APIs", "alloc init failed\n");
    }

    return result;
}

// Main entry point
int main(int argc, char *argv[]) {
    printf("Starting ISP initialization...\n");
    printf("ISP physical base address: 0x%08x\n", ISP_BASE_ADDR);

    // Open memory device
    mem_fd = open("/dev/mem", O_RDWR | O_SYNC);
    if (mem_fd < 0) {
        perror("Failed to open /dev/mem");
        return 1;
    }

    // Map the ISP memory region
    map_base = mmap(NULL, ISP_MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, mem_fd, ISP_BASE_ADDR);
    if (map_base == MAP_FAILED) {
        perror("Failed to mmap");
        cleanup();
        return 1;
    }

    printf("Memory mapped successfully at %p\n", map_base);

    printf("Detecting cpu info...\n");
    read_soc_info();
    // get_cpuinfo();

    // Example parameters for testing
    struct tisp_param_info param = {.data = {0x01, 0x02, 0x03, 0x04}};
    const char *bin_path = argc > 1 ? argv[1] : "";

    // Initialize the ISP with the provided parameters
    if (tisp_init(&param, bin_path) != 0) {
        printf("ISP initialization failed!\n");
        cleanup();
        return 1;
    }

    printf("\nISP initialization completed\n");
    printf("Control Register: 0x%08x\n", read_isp_register(REG_CONTROL));
    printf("Bayer Pattern Register: 0x%08x\n", read_isp_register(REG_BAYER_PATTERN));
    printf("DEIR Control Register: 0x%08x\n", read_isp_register(REG_DEIR_CONTROL));
    printf("Status Register: 0x%08x\n", read_isp_register(REG_STATUS));

    IMP_ISP_Open();
    IMP_ISP_AddSensor("sc2336");

    // Cleanup resources
    cleanup();
    return 0;
}

// Cleanup function to release resources
void cleanup() {
    if (map_base != MAP_FAILED) {
        munmap(map_base, ISP_MAP_SIZE);
    }
    if (mem_fd > 0) {
        close(mem_fd);
    }
}