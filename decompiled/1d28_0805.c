
void __cdecl16far FUN_1d28_0805(int *param_1,int *param_2,int *param_3)

{
  uint uVar1;
  int iVar2;
  uint uVar3;
  int iVar4;
  int iVar5;
  
  uVar3 = 0;
  iVar2 = 0;
  iVar5 = 0;
  iVar4 = DAT_4125_327a;
  do {
    uVar1 = *(int *)&DAT_4125_0002 - 1;
    if ((((char *)s_Borland_C_____Copyright_1991_Bor_4125_0004)[4] & 0x80U) == 0) {
      iVar2 = iVar2 + uVar1;
      if (uVar3 <= uVar1) {
        uVar3 = uVar1;
      }
    }
    else if ((((char *)s_Borland_C_____Copyright_1991_Bor_4125_0004)[4] & 0x20U) != 0) {
      iVar5 = iVar5 + uVar1;
    }
    iVar4 = iVar4 + uVar1 + 1;
  } while (iVar4 != DAT_4125_327e);
  *param_1 = iVar2 << 4;
  ((int *)param_1)[1] =
       (((uint)(iVar2 < 0) << 1 | (uint)(iVar2 << 1 < 0)) << 1 | (uint)(iVar2 << 2 < 0)) << 1 |
       (uint)(iVar2 << 3 < 0);
  *param_2 = uVar3 << 4;
  ((int *)param_2)[1] =
       (((uint)((int)uVar3 < 0) << 1 | (uint)((int)(uVar3 << 1) < 0)) << 1 |
       (uint)((int)(uVar3 << 2) < 0)) << 1 | (uint)((int)(uVar3 << 3) < 0);
  *param_3 = iVar5 << 4;
  ((int *)param_3)[1] =
       (((uint)(iVar5 < 0) << 1 | (uint)(iVar5 << 1 < 0)) << 1 | (uint)(iVar5 << 2 < 0)) << 1 |
       (uint)(iVar5 << 3 < 0);
  return;
}


