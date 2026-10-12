
void __cdecl16far auto_str_out_of_memory_at(undefined4 param_1)

{
  char *fmt;
  
  printf(s_Out_of_memory_at__lu_4125_2848,param_1);
  auto_str_conv_lu_maxblock_lu();
  if (DAT_4125_3280 - 1U < 6) {
                    /* WARNING: Switch is manually overridden */
    switch((undefined4)*(undefined2 *)((DAT_4125_3280 - 1U) * 2 + 0x3e5)) {
    case 0x1a870:
      fmt = s_Out_of_mem_handles_4125_285e;
      break;
    case 0x1a876:
      fmt = s_Out_of_memory_4125_2871;
      break;
    case 0x1a87c:
      fmt = s_Too_much_memory_locked_4125_287f;
      break;
    case 0x1a882:
      fmt = s_Bad_handle_4125_2896;
      break;
    case 0x1a888:
      fmt = s_Invalid_handle_4125_28a1;
      break;
    case 0x1a88e:
      fmt = s_Block_handle_mismatch_4125_28b0;
    }
    printf(fmt);
  }
  getch();
  auto_str_warlords_c_strategic_studies();
  return;
}


