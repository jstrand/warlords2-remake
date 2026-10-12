
undefined2 __cdecl16far FUN_1d28_08a3(undefined4 param_1,ulong param_2)

{
  int iVar1;
  int iVar2;
  uint uVar3;
  int iVar4;
  undefined2 extraout_DX;
  undefined2 extraout_DX_00;
  int iVar5;
  undefined2 uVar6;
  uint uVar7;
  int iVar8;
  bool bVar9;
  
  uVar6 = (undefined2)((ulong)param_1 >> 0x10);
  iVar5 = (int)param_1;
  mem_lock(iVar5,uVar6);
  uVar7 = (uint)(param_2 >> 0x10);
  if (((int)param_2 == 0 && uVar7 == 0) ||
     (iVar2 = (int)(((ulong)((uVar7 >> 3 & 1) != 0) << 0x10 |
                    ((ulong)((uVar7 >> 2 & 1) != 0) << 0x10 |
                    ((ulong)((uVar7 >> 1 & 1) != 0) << 0x10 |
                    ((ulong)((param_2 & 0x10000) != 0) << 0x10 | param_2 & 0xffff) >> 1) >> 1) >> 1)
                   >> 1), uVar7 >> 4 != 0)) {
LAB_1d28_0a29:
    uVar6 = 0;
  }
  else {
    if ((param_2 & 0xf) != 0) {
      iVar2 = iVar2 + 1;
    }
    uVar3 = iVar2 + 1;
    iVar2 = *(int *)(iVar5 + 2) + -1;
    uVar7 = *(uint *)&DAT_4125_0002;
    if (uVar7 < uVar3) {
      iVar1 = *(int *)&DAT_4125_0002;
      iVar4 = iVar2 + iVar1;
      bVar9 = iVar4 == DAT_4125_327e;
      iVar8 = iVar2;
      if ((!bVar9) &&
         (bVar9 = false, iVar8 = iVar4,
         (((char *)s_Borland_C_____Copyright_1991_Bor_4125_0004)[4] & 0x80U) == 0)) {
        uVar7 = *(int *)&DAT_4125_0002 + iVar1;
        bVar9 = uVar7 == uVar3;
        if (uVar3 <= uVar7) {
          if ((uVar3 < uVar7) && (uVar7 - uVar3 != 1)) {
            DAT_4125_327c = iVar2;
            *(uint *)&DAT_4125_0002 = uVar3;
            iVar5 = uVar3 + iVar2;
            uVar6 = FUN_1d28_068c();
            *(int *)0x0 = iVar2;
            *(undefined2 *)&DAT_4125_0002 = uVar6;
          }
          else {
            *(int *)&DAT_4125_0002 = *(int *)&DAT_4125_0002 + iVar1;
            iVar5 = iVar2;
          }
          iVar2 = iVar5 + *(int *)&DAT_4125_0002;
          if (iVar2 != DAT_4125_327e) {
            *(int *)0x0 = iVar5;
          }
          goto LAB_1d28_0a2e;
        }
      }
      FUN_1d28_053c(iVar2);
      if (bVar9) goto LAB_1d28_0a29;
      DAT_4125_327c = iVar8;
      FUN_1d28_0be7(extraout_DX,0x4125);
      FUN_1d28_060d();
      *(int *)(iVar5 + 2) = iVar8 + 1;
    }
    else if ((uVar7 != uVar3) && (uVar7 - uVar3 != 1)) {
      iVar5 = *(int *)&DAT_4125_0002 - (uVar7 - uVar3);
      DAT_4125_327c = iVar2;
      *(int *)&DAT_4125_0002 = iVar5;
      iVar5 = iVar2 + iVar5;
      FUN_1d28_068c();
      *(int *)0x0 = iVar2;
      *(undefined2 *)&DAT_4125_0002 = extraout_DX_00;
      iVar2 = iVar5 + *(int *)&DAT_4125_0002;
      if (iVar2 != DAT_4125_327e) {
        *(int *)0x0 = iVar5;
      }
    }
LAB_1d28_0a2e:
    uVar6 = 0xffff;
  }
  return uVar6;
}


