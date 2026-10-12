
void __xcvt(uint param_1)

{
  uint *puVar1;
  undefined4 in_stack_0000000e;
  
  puVar1 = (uint *)(param_1 + (int)in_stack_0000000e);
  *puVar1 = *puVar1 & 0x7fff;
                    /* WARNING: Could not recover jumptable at 0x000116da. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **)((char *)s_Button_file_not_found_4125_16da + 5 + (param_1 >> 2) * 2))();
  return;
}


