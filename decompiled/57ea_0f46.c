
void __cdecl16far FUN_57ea_0f46(int param_1,int param_2)

{
  byte *pbVar1;
  uint *puVar2;
  int iVar3;
  undefined2 unaff_SS;
  undefined1 local_70 [100];
  undefined2 local_c;
  undefined2 local_a;
  int local_8;
  undefined2 local_6;
  undefined2 local_4;
  
  iVar3 = FUN_57ea_177f(param_1,&local_4);
  if (iVar3 == 0) {
    if ((((undefined1 *)&army_move_flags)[*(char *)(param_1 * 0x16 + -0x777c) * 6] == '\0') ||
       (iVar3 = FUN_57ea_164e(param_1,&local_4), iVar3 == 0)) {
      iVar3 = 100;
      while (iVar3 != 0) {
        local_70[iVar3 + -1] = 0;
        iVar3 = iVar3 + -1;
      }
      if ((param_2 == -1) || (iVar3 = FUN_57ea_0935(param_2,local_70), iVar3 == 0xff)) {
        pbVar1 = (byte *)(param_1 * 0x16 + -0x7772);
        *pbVar1 = *pbVar1 & 0xf0;
        pbVar1 = (byte *)(param_1 * 0x16 + -0x7771);
        *pbVar1 = *pbVar1 & 1;
        puVar2 = (uint *)(param_1 * 0x16 + -0x7774);
        *puVar2 = *puVar2 & 0xffdf;
        *(undefined2 *)(param_1 * 0x16 + -0x776c) = 0xffff;
        *(undefined2 *)(param_1 * 0x16 + -0x776e) = 0xffff;
        *(undefined1 *)(param_1 * 0x16 + -0x776f) = 0;
        return;
      }
      local_6 = *(undefined2 *)(iVar3 * 0x41 + 0x157f);
      local_4 = *(undefined2 *)(iVar3 * 0x41 + 0x157d);
    }
  }
  else if ((local_8 != 0) &&
          ((local_8 != 1 || (iVar3 = FUN_57ea_10a8(param_1,local_a,local_c), iVar3 != 0)))) {
    if (local_8 != 2) {
      return;
    }
    iVar3 = FUN_57ea_12eb(param_1,local_a,local_c);
    if (iVar3 != 0) {
      return;
    }
  }
  FUN_57ea_1581(param_1,local_4,local_6);
  return;
}


