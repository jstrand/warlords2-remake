
/* WARNING: Removing unreachable block (ram,0x000262e1) */

undefined4 FUN_25de_04f8(void)

{
  undefined2 in_AX;
  undefined2 in_DX;
  undefined2 *in_stack_00000000;
  
  if ((undefined2 *)in_stack_00000000 == (undefined2 *)&DAT_4125_0002) {
    FUN_25de_05a8(*in_stack_00000000);
  }
  else {
    LOCK();
    UNLOCK();
    FUN_25de_05a8(*in_stack_00000000);
    LOCK();
    UNLOCK();
  }
  ((char *)s_Borland_C_____Copyright_1991_Bor_4125_0004)[0x16] =
       ((char *)s_Borland_C_____Copyright_1991_Bor_4125_0004)[0x16] & 0xf7;
  (*(code *)*(undefined2 *)0x86)(0x2000);
  return CONCAT22(in_DX,in_AX);
}


