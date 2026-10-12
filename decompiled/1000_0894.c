
undefined2 __cdecl16far set_timer_isr(undefined2 param_1,undefined2 param_2,undefined2 param_3)

{
  undefined1 uVar1;
  
  uRam00000020 = param_1;
  uRam00000022 = param_2;
  out(0x43,0x36);
  out(0x40,(char)param_3);
  uVar1 = (undefined1)((uint)param_3 >> 8);
  out(0x40,uVar1);
  return CONCAT11(uVar1,uVar1);
}


