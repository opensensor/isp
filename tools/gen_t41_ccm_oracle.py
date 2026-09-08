#!/usr/bin/env python3
"""Offline CCM test oracle: private userspace objects, no ISP access."""
import hashlib
import struct
import sys
from elftools.elf.elffile import ELFFile

with open(sys.argv[1], 'rb') as source:
    assert hashlib.file_digest(source, 'sha256').hexdigest() == \
        '572ff4553c1a033290ec67d2d9fc384701fbc235c2e335c7e5604100966fb2ee'
    source.seek(0)
    elf = ELFFile(source)
    code = elf.get_section_by_name('.text').data()
    symbols = elf.get_section_by_name('.symtab')
    functions = {
        'tisp_round_int64': 'oracle_round',
        'tisp_ccm_interp_by_ct': 'oracle_ct',
        'tisp_ccm_interp_by_ev': 'oracle_ev',
        'tisp_ccm_matrix_trans_by_sat': 'oracle_saturate',
        'tisp_ccm_write_reg': 'oracle_pack',
    }
    bcsh = len(sys.argv) > 2 and sys.argv[2] == 'bcsh'
    ae = len(sys.argv) > 2 and sys.argv[2] == 'ae'
    gib = len(sys.argv) > 2 and sys.argv[2] == 'gib'
    awb_gain = len(sys.argv) > 2 and sys.argv[2] == 'awb_gain'
    awb_stats = len(sys.argv) > 2 and sys.argv[2] == 'awb_stats'
    awb_long = len(sys.argv) > 2 and sys.argv[2] == 'awb_long'
    awb_detect = len(sys.argv) > 2 and sys.argv[2] == 'awb_detect'
    awb_frame = len(sys.argv) > 2 and sys.argv[2] == 'awb_frame'
    awb_control = len(sys.argv) > 2 and sys.argv[2] == 'awb_control'
    gamma = len(sys.argv) > 2 and sys.argv[2] == 'gamma'
    dpc = len(sys.argv) > 2 and sys.argv[2] == 'dpc'
    dmsc = len(sys.argv) > 2 and sys.argv[2] == 'dmsc'
    sdns = len(sys.argv) > 2 and sys.argv[2] == 'sdns'
    ydns = len(sys.argv) > 2 and sys.argv[2] == 'ydns'
    ysp = len(sys.argv) > 2 and sys.argv[2] == 'ysp'
    cdns = len(sys.argv) > 2 and sys.argv[2] == 'cdns'
    mdns = len(sys.argv) > 2 and sys.argv[2] == 'mdns'
    lce = len(sys.argv) > 2 and sys.argv[2] == 'lce'
    adr = len(sys.argv) > 2 and sys.argv[2] == 'adr'
    lsc = len(sys.argv) > 2 and sys.argv[2] == 'lsc'
    if bcsh:
        functions = dict(zip([
            'tisp_round_int64', 'tisp_min', 'tisp_max', 'tisp_bcsh_itp',
            'tisp_bcsh_matrix_multi', 'tisp_bcsh_interp_by_ct',
            'tisp_bcsh_interp_by_ev', 'tisp_bcsh_BCS_adjust',
            'tisp_bcsh_H_adjust', 'tisp_bcsh_aitp_to_hard', 'tisp_bcsh_write_reg',
        ], ['oracle_round', 'oracle_min', 'oracle_max', 'oracle_itp',
            'oracle_multiply', 'oracle_ct', 'oracle_ev', 'oracle_bcs',
            'oracle_hue', 'oracle_hard', 'oracle_pack']))
    if ae:
        functions = dict(zip([
            'tisp_ae_long_target', 'tisp_ae_lib_bilinear_intp',
            'tisp_ae_lib_div_64', 'tisp_ae_get_statistics',
            'tisp_ae_weight_mean', 'fix_point_div_32', 'fix_point_mult2_32',
        ], ['oracle_target', 'oracle_interpolate', 'oracle_divide',
            'oracle_statistics', 'oracle_mean', 'oracle_fixed_div',
            'oracle_fixed_mul']))
    if gib:
        functions = dict(zip(['tisp_gib_calc_self_gain', 'tisp_gib_ae_write_dgain',
            'tisp_round_int64', 'tisp_max'],
            ['oracle_self_gain', 'oracle_dgain', 'oracle_round', 'oracle_max']))
    if awb_gain:
        functions = dict(zip(['tisp_awb_gain_reg', 'tisp_awb_set_gain', 'fix_point_mult2_32'],
            ['oracle_pack', 'oracle_gain', 'oracle_fixed_mul']))
    if awb_stats or awb_detect or awb_frame:
        functions = {name: 'oracle_' + name for name in [
            'tisp_awb_get_statistics', 'tisp_awb_sat_weight', 'tisp_awb_long_par_update',
            'tisp_awb_spec_calculate', 'fix_point_mult2_32', 'fix_point_div_32',
            'ISPAWBInterpolation1', 'ISPAWBInterpolation2', 'Tiziano_Awb_Ct_Cal',
            'fix_point_mult3_32', 'Tiziano_Awb_Ct_Detect_GrayWorld_mode',
            'Tiziano_Awb_Ct_Detect_GrayWorld']}
        if awb_detect or awb_frame:
            functions.update({name: 'oracle_' + name for name in [
                'tisp_awb_ct_detect', 'func_zone_ct_weight']})
        if awb_frame:
            functions.update({name: 'oracle_' + name for name in [
                'tisp_awb_long_alogrithm', 'fix_point_div', 'tisp_awb_set_gain', 'tisp_awb_gain_reg']})
    if awb_long:
        functions = {name: 'oracle_' + name for name in [
            'tisp_awb_long_alogrithm', 'tisp_awb_long_par_update',
            'tisp_awb_sat_weight', 'fix_point_mult2_32', 'fix_point_div']}
    if awb_control:
        functions = {name: 'oracle_' + name for name in [
            'tisp_awb_init', 'tisp_awb_params_refresh', 'tisp_awb_show_para_update',
            'tisp_awb_ev_update_Ywgt', 'tisp_awb_ev_update', 'tisp_awb_dn_params_refresh',
            'tisp_awb_param_array_get', 'tisp_awb_param_array_set', 'tisp_awb_get_attr',
            'tisp_awb_get_global_statis', 'tisp_awb_set_weight', 'tisp_awb_get_weight',
            'tisp_awb_get_zone', 'tisp_awb_set_mode', 'tisp_awb_get_mode',
            'tisp_awb_set_frz', 'tisp_awb_get_frz', 'tisp_awb_set_ct',
            'tisp_awb_set_statis_localtion', 'tisp_awb_api_set_ct_trend_offset',
            'tisp_awb_api_get_ct_trend_offset', 'tisp_awb_set_converge_step',
            'tisp_awb_get_converge_step']}
    if gamma:
        functions = dict(zip(['tisp_gamma_interp_by_ev', 'tisp_gamma_strength_transform',
            'tisp_gamma_write_lut_rgb', 'tisp_round_int64'],
            ['oracle_select', 'oracle_curve', 'oracle_pack', 'oracle_round']))
    if dpc:
        functions = dict(zip(['tisp_dpc_gain_interp', 'tisp_dpc_write_reg_long',
            'tisp_dpc_write_reg_short', 'tisp_simple_intp_int16', 'tisp_simple_intp_int8',
            'tisp_dpc_write_reg_other'],
            ['oracle_interpolate', 'oracle_long', 'oracle_short', 'oracle_lerp16', 'oracle_lerp8', 'oracle_other']))
    if dmsc:
        functions = dict(zip(['tisp_dmsc_intp', 'tisp_dmsc_noref_reg_cfg',
            'tisp_dmsc_ref_reg_cfg', 'tisp_simple_intp_int16', 'tisp_simple_intp_int8',
            'tisp_ratio'],
            ['oracle_interpolate', 'oracle_static', 'oracle_dynamic', 'oracle_lerp16', 'oracle_lerp8', 'oracle_ratio']))
    if sdns:
        functions = dict(zip(['tisp_sdns_intp', 'tisp_sdns_noref_reg_cfg',
            'tisp_sdns_ref_reg_cfg', 'tisp_simple_intp_int16', 'tisp_simple_intp_int8',
            'tisp_ratio'],
            ['oracle_interpolate', 'oracle_static', 'oracle_dynamic', 'oracle_lerp16', 'oracle_lerp8', 'oracle_ratio']))
    if ydns:
        functions = dict(zip(['tisp_ydns_intp', 'tisp_ydns_reg_cfg', 'tisp_simple_intp_int8'],
            ['oracle_interpolate', 'oracle_pack', 'oracle_lerp8']))
    if ysp:
        functions = dict(zip(['tisp_ysp_intp', 'tisp_ysp_noref_reg_cfg',
            'tisp_ysp_ref_reg_cfg', 'tisp_simple_intp_int16', 'tisp_simple_intp_int8',
            'tisp_ysp_sharpness_set'],
            ['oracle_interpolate', 'oracle_static', 'oracle_dynamic', 'oracle_lerp16', 'oracle_lerp8', 'oracle_strength']))
    if cdns:
        functions = dict(zip(['tisp_cdns_intp', 'tisp_cdns_reg_cfg', 'tisp_simple_intp_int8'],
            ['oracle_interpolate', 'oracle_pack', 'oracle_lerp8']))
    if mdns:
        functions = dict(zip(['tisp_mdns_intp', 'tisp_mdns_reg_cfg', 'tisp_simple_intp_int8',
            'tisp_mdns_reg_cfg_equation_smp', 'tisp_mdns_reg_cfg_equation_dif', 'tisp_mdns_func_en',
            'tisp_s_mdns_ratio'],
            ['oracle_interpolate', 'oracle_pack', 'oracle_lerp8', 'oracle_smp', 'oracle_dif', 'oracle_enable',
             'oracle_strength']))
    if lce:
        functions = {name: 'oracle_' + name for name in [
            'tisp_lce_init', 'tisp_lce_awdr_to_used', 'tisp_lce_tgain_interp_strength',
            'tisp_simple_intp_int8', 'tisp_lce_curve_init_default', 'tisp_lce_write_all_reg',
            'get_distance_1dim', 'lce_hist_filter_and_judge', 'lce_head_tail_search',
            'lce_hist_method', 'lce_pdf_to_cdf', 'lce_self_light_correct',
            'lce_16bit_data_converge', 'lce_wdr_light_lock', 'lce_light_lock_adjust_hist',
            'lce_std_hist_transform', 'Tisp_lce_soft']}
    if adr:
        functions = {name: 'oracle_' + name for name in [
            'func_adr_reg_write_one', 'func_adr_reg_write_5x5',
            'func_adr_reg_write_sometimes', 'func_adr_reg_write_every',
            'tiziano_adr_read_data', 'tiziano_adr_stat_calc',
            'tiziano_adr_5x5_out', 'tiziano_adr_5x5_init', 'tiziano_adr_base_pars',
            'func_gauss_local', 'fix_point_div_32', 'fix_point_mult2_32', 'tisp_math_exp2',
            'ispint_adr_64', 'tiziano_adr_ev_func', 'fix_point_div',
            'ispint_adr_16', 'func_interp1_short', 'func_gam_x2y', 'func_local_info',
            'subsection_map', 'subsection', 'subsection_up', 'func_adr_map_curve1',
            'func_map_y_filter', 'func_map_y_filter_sp', 'Tiziano_adr_fpga',
            'tiziano_adr_algorithm', 'tisp_adr_faceae_curve_adjust', 'tiziano_adr_hardpars_ctl',
            'tisp_adr_init', 'tiziano_adr_gamma_refresh', 'tisp_adr_dn_params_refresh', 'tisp_adr_linear_switch']}
    if lsc:
        functions = {name: 'oracle_' + name for name in [
            'tisp_lsc_ct_interp', 'tisp_lsc_gain_interp', 'tisp_lsc_mesh_itp',
            'tisp_lsc_ring_itp', 'tisp_lsc_itp', 'tisp_simple_intp_int8',
            'tisp_max', 'tisp_lsc_write_reg', 'tisp_lsc_lut_valid_judge',
            'tisp_lsc_mirror_flip', 'lsc_exchange_data.constprop.1', 'tisp_lsc_real_write_lut',
            'tisp_lsc_init', 'tisp_lsc_wdr_en', 'tisp_lsc_dn_params_refresh',
            'tisp_lsc_param_array_set', 'tisp_lsc_prepare_write_lut']}
    names = dict(functions, **{'.bss': 'oracle_bss', 'memcpy': 'oracle_copy',
        'memset': 'oracle_fill', '__ashrdi3': 'oracle_signed_shift',
        'system_reg_write': 'oracle_write', '.rodata': 'oracle_rodata',
        'isp_printf': 'oracle_unexpected'})
    if awb_gain:
        names['system_reg_set_awb_trig'] = 'oracle_trigger'
    if awb_stats or awb_long or awb_detect or awb_frame:
        names.update({'awb_list_cluster_rg': 'oracle_cluster_red',
                      'awb_list_cluster_bg': 'oracle_cluster_blue', 'system_reg_read': 'oracle_read',
                      '__ashldi3': 'oracle_left_shift', '__lshrdi3': 'oracle_shift',
                      '__div64_32': 'oracle_div64'})
    if awb_long:
        names.update({'tisp_awb_ct_detect': 'oracle_detector',
                      'div64_u64': 'oracle_div64_u64'})
    if awb_frame:
        names['div64_u64'] = 'oracle_div64_u64'
        names['system_reg_set_awb_trig'] = 'oracle_trigger'
    if awb_control:
        names.update({'tparamsP': 'oracle_params', 'tpm_cb': 'oracle_callbacks',
            'awb_ls_wgt_lut_inter': 'oracle_distance_reference',
            'private_kmalloc': 'oracle_kmalloc', 'private_kfree': 'oracle_kfree',
            'tisp_awb_set_hardware_param': 'oracle_hardware', 'tisp_awb_set_gain': 'oracle_gain',
            **{name: 'oracle_noop' for name in ['system_irq_func_set', 'tisp_event_set_cb',
                'tisp_awb_main_interrupt_static', 'tisp_awb_process',
                'tisp_awb_pm_get_regsize', 'tisp_awb_pm_suspend', 'tisp_awb_pm_resume']}})
    if lsc:
        names.update({'lsc_slock': 'oracle_bss+0x4130',
            '__private_spin_lock_irqsave': 'oracle_noop', 'private_spin_unlock_irqrestore': 'oracle_noop',
            'system_reg_read': 'oracle_read', '__div64_32': 'oracle_div64',
            'tparamsP': 'oracle_params', 'tpm_cb': 'oracle_callbacks', 'private_kmalloc': 'oracle_kmalloc',
            **{name:'oracle_noop' for name in ['private_spin_lock_init','system_irq_func_set',
                'tisp_lsc_fra_down_interrupt','tisp_lsc_pm_get_regsize','tisp_lsc_pm_suspend','tisp_lsc_pm_resume']}})
    if adr:
        names.update({'system_reg_read': 'oracle_read', '.data': 'oracle_data',
            'adr_5x5_in2': 'oracle_radial_reference',
            'ai_curve1_y': 'oracle_data+0x42e4',
            'adr_imgDR_w': 'oracle_data+0x40e0', 'sumImgDR_w': 'oracle_data+0x42e0',
            'adr_kneepoint_x': 'oracle_data+0x4894', 'adr_kneepoint_diff': 'oracle_data+0x48b4',
            'adr_local_light_b_in': 'oracle_data+0x48d4', 'adr_top_gam_x': 'oracle_data+0x4760',
            'adr_ctc1_x': 'oracle_data+0x4864',
            'adr_gauss_local': 'oracle_bss+0x42b0', 'adr_cut_base_up': 'oracle_bss+0x4260',
            'adr_cut_base': 'oracle_bss+0x428c', 'adr_count': 'oracle_bss+0x4344',
            'adr_faceae_en': 'oracle_bss+0x4600', 'adr_faceae_blockflag': 'oracle_bss+0x45e8',
            'adr_faceae_map_curve': 'oracle_bss+0x4348',
            'adr_gauss_old': 'oracle_gauss_old',
            '__lshrdi3': 'oracle_shift',
            '__ashldi3': 'oracle_left_shift',
            'div64_u64': 'oracle_div64_u64',
            'private_vmalloc': 'oracle_alloc', 'private_vfree': 'oracle_free'})
        names.update({'tparamsP': 'oracle_params', 'tpm_cb': 'oracle_callbacks', '.sbss': 'oracle_mutex_key',
            'private_kmalloc': 'oracle_kmalloc',
            **{name: 'oracle_noop' for name in ['private_raw_mutex_init', 'system_irq_func_set',
                'tisp_event_set_cb', 'tiziano_adr_interrupt_static', 'tisp_adr_process_func',
                'tisp_adr_pm_get_regsize', 'tisp_adr_pm_suspend', 'tisp_adr_pm_resume']}})
    if ysp:
        names.update({'ysp_paramsP': 'oracle_original', '.data': 'oracle_data',
            'tisp_ysp_all_reg_refresh': 'oracle_noop'})
    if mdns:
        names.update({'get_isp_memopt': 'oracle_memopt', 'tparamsP': 'oracle_params',
            'tparams_day': 'oracle_day', 'tparams_night': 'oracle_night',
            'tisp_get_tuning': 'oracle_tuning', 'tisp_mdns_refresh': 'oracle_noop'})
    if lce:
        names.update({'private_vmalloc': 'oracle_alloc', 'tparamsP': 'oracle_params',
            'tpm_cb': 'oracle_callbacks',
            '__lshrdi3': 'oracle_shift', 'private_spin_lock_init': 'oracle_noop',
            '__private_spin_lock_irqsave': 'oracle_noop', 'private_spin_unlock_irqrestore': 'oracle_noop',
            **{name: 'oracle_noop' for name in ['tisp_lce_clr_ram.part.0', 'tisp_lce_clr_ram',
                'system_irq_func_set', 'tisp_event_set_cb', 'tisp_lce_interrupt_static',
                'tisp_lce_process', 'tisp_lce_pm_get_regsize', 'tisp_lce_pm_suspend', 'tisp_lce_pm_resume']}})
    if ae:
        names.update({'__ashldi3': 'oracle_left_shift',
            '__lshrdi3': 'oracle_shift', '__div64_32': 'oracle_div64',
            'y_arr': 'oracle_y',
            'tisp_ae_fliker_detect': 'oracle_noop',
            'tisp_ae_get_bv': 'oracle_noop'})
    rels = {r['r_offset']: r for r in elf.get_section_by_name('.rel.text').iter_relocations()}
    local_relocs = {}
    if lce or lsc or awb_control:
        for pc, rel in rels.items():
            entry = symbols.get_symbol(rel['r_info_sym'])
            section_name = entry.name or elf.get_section(entry['st_shndx']).name
            if section_name != '.text' or rel['r_info_type'] != 5:
                continue
            high, = struct.unpack_from('<I', code, pc)
            for lo_pc in range(pc + 4, pc + 128, 4):
                lo_rel = rels.get(lo_pc)
                if not lo_rel or lo_rel['r_info_sym'] != rel['r_info_sym'] or lo_rel['r_info_type'] != 6:
                    continue
                low, = struct.unpack_from('<I', code, lo_pc)
                if (low >> 21 & 31) != (high >> 16 & 31):
                    continue
                address = ((high & 65535) << 16) + ((low & 32767) - (low & 32768))
                matches = [s.name for s in symbols.iter_symbols() if s['st_value'] == address
                           and s['st_info']['type'] == 'STT_FUNC' and s.name in names]
                if matches:
                    local_relocs[pc] = local_relocs[lo_pc] = names[matches[0]]
                break
    print('.set noreorder\n.set noat\n.option pic0\n.text\n.balign 4')
    for original, name in functions.items():
        symbol, = symbols.get_symbol_by_name(original)
        start, size = symbol['st_value'], symbol['st_size']
        print(f'.globl {name}\n.type {name}, @function\n{name}:')
        for pc in range(start, start + size, 4):
            word, = struct.unpack_from('<I', code, pc)
            if pc not in rels:
                print(f'.word 0x{word:08x} # {pc:05x}')
                continue
            rel = rels[pc]
            entry = symbols.get_symbol(rel['r_info_sym'])
            if pc in local_relocs:
                rt, rs = word >> 16 & 31, word >> 21 & 31
                target = local_relocs[pc]
                print(f'lui ${rt}, %hi({target})' if rel['r_info_type'] == 5 else
                      f'addiu ${rt}, ${rs}, %lo({target})')
                continue
            target = 'oracle_message' if entry.name.startswith('$LC') else \
                names[entry.name or elf.get_section(entry['st_shndx']).name]
            kind = rel['r_info_type']
            rs, rt, imm = word >> 21 & 31, word >> 16 & 31, word & 65535
            if kind == 5:
                assert imm == 0
                print(f'lui ${rt}, %hi({target})')
            elif kind == 6:
                assert imm < 0x8000
                if word >> 26 == 9:
                    print(f'addiu ${rt}, ${rs}, %lo({target}+{imm})')
                else:
                    op = {33: 'lh', 35: 'lw', 36: 'lbu', 37: 'lhu', 41: 'sh', 43: 'sw'}[word >> 26]
                    print(f'{op} ${rt}, %lo({target}+{imm})(${rs})')
            else:
                raise ValueError((hex(pc), kind))
        print(f'.size {name}, .-{name}')
    if awb_stats or awb_detect or awb_control:
        table, = symbols.get_symbol_by_name('awb_ls_wgt_lut_inter')
        values = struct.unpack_from('<514H', elf.get_section(table['st_shndx']).data(), table['st_value'])
        print('.section .rodata\n.balign 4\n.globl oracle_distance_reference\noracle_distance_reference:')
        for offset in range(0, len(values), 16):
            print('.hword ' + ','.join(map(str, values[offset:offset+16])))
    if lsc:
        # Relocate the setter's five-entry switch table, not its branch logic.
        symbol, = symbols.get_symbol_by_name('tisp_lsc_param_array_set')
        rodata = elf.get_section_by_name('.rodata').data()
        print('.section .rodata\n.balign 4\n.globl oracle_lsc_set_cases\noracle_lsc_set_cases:')
        for target in struct.unpack_from('<5I', rodata, 0x1e80):
            offset = target - symbol['st_value']
            assert 0 <= offset < symbol['st_size']
            print(f'.word oracle_tisp_lsc_param_array_set+{offset}')
    if awb_control:
        # Preserve the real setter switch branches with relocated code pointers.
        symbol, = symbols.get_symbol_by_name('tisp_awb_set_mode')
        data = elf.get_section_by_name('.rodata').data()
        print('.section .rodata\n.balign 65536\n.globl oracle_rodata\noracle_rodata:')
        for pos in range(0, len(data), 4):
            if 0x1e10 <= pos < 0x1e38:
                target, = struct.unpack_from('<I', data, pos)
                offset = target-symbol['st_value']
                assert 0 <= offset < symbol['st_size']
                print(f'.word oracle_tisp_awb_set_mode+{offset}')
            else:
                print('.byte '+','.join(str(v) for v in data[pos:pos+4]))
        print('oracle_message:\n.asciz "unexpected reference diagnostic"')
    if ae:
        # The target-table adjustment is inlined in tisp_ae_tune. This exact
        # bounded, call-free loop gets private caller-owned arrays, a private
        # calibration copy and a normal o32 stack; no surrounding AE routine.
        print('''\n.globl oracle_adjust
.type oracle_adjust, @function
oracle_adjust:
addiu $sp, $sp, -256
sw $s6, 240($sp)
move $s6, $a0
move $v1, $a1
move $a1, $a2
addiu $a0, $s6, 0x78
addiu $t1, $s6, 0x208''')
        for pc in range(0x272f4, 0x273d8, 4):
            assert pc not in rels
            word, = struct.unpack_from('<I', code, pc)
            print(f'.word 0x{word:08x} # {pc:05x}')
        print('''lw $s6, 240($sp)
jr $ra
addiu $sp, $sp, 256
.size oracle_adjust, .-oracle_adjust''')
    if bcsh or lce or adr:
        print('.section .rodata\n.balign 65536\n.globl oracle_rodata\noracle_rodata:')
        data = elf.get_section_by_name('.rodata').data()
        for pos in range(0, len(data) if lce or adr else 0x2a70, 16):
            print('.byte ' + ','.join(str(v) for v in data[pos:pos+16]))
        print('oracle_message:\n.asciz "unexpected reference diagnostic"')
    if adr:
        print('.data\n.balign 65536\n.globl oracle_data\noracle_data:')
        data = elf.get_section_by_name('.data').data()
        for pos in range(0, len(data), 16):
            print('.byte ' + ','.join(str(v) for v in data[pos:pos+16]))
        radial, = symbols.get_symbol_by_name('adr_5x5_in2')
        data = elf.get_section(radial['st_shndx']).data()
        print('.balign 4\n.globl oracle_radial_reference\noracle_radial_reference:')
        for i in range(31):
            value, = struct.unpack_from('<I', data, radial['st_value'] + 4*i)
            print(f'.word {value}')
    print('.section .note.GNU-stack,"",@progbits')
