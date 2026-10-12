
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl16far auto_file_current_sgn_2(void)

{
  bool bVar1;
  int *piVar2;
  int *piVar3;
  int iVar4;
  int iVar5;
  undefined2 uVar6;
  int *piVar7;
  void *pvVar8;
  int iVar9;
  uint unaff_DI;
  long lVar10;
  
  bVar1 = false;
  if (((DAT_451b_1f02 != (int *)0x0) &&
      (*(char *)(*(byte *)(((int *)DAT_451b_1f02)[1] * 0xe0 + *DAT_451b_1f02 * 2) + 0x710) == '\t'))
     && (iVar5 = open(s_current_sgn_4125_03df,2), -1 < iVar5)) {
    lVar10 = filelength(iVar5);
    pvVar8 = (void *)((ulong)lVar10 >> 0x10);
    uVar6 = (undefined2)lVar10;
    DAT_451b_003c = uVar6;
    piVar7 = (int *)FUN_1d28_01d5(uVar6);
    DAT_451b_003e = (int *)CONCAT22(pvVar8,piVar7);
    read(iVar5,(void *)CONCAT22(uVar6,pvVar8),unaff_DI);
    close(iVar5);
    piVar3 = DAT_451b_003e;
    iVar5 = *DAT_451b_003e;
    piVar2 = (int *)DAT_451b_003e;
    piVar7 = (int *)DAT_451b_003e + 1;
    uVar6 = DAT_451b_003e._2_2_;
    DAT_451b_003e = (int *)CONCAT22(DAT_451b_003e._2_2_,piVar7);
    _DAT_451b_0036 = DAT_451b_003e;
    iVar4 = 0;
    do {
      iVar9 = iVar4;
      if ((piVar7[iVar9 * 0x34] == *DAT_451b_1f02) &&
         (piVar7[iVar9 * 0x34 + 1] == ((int *)DAT_451b_1f02)[1])) {
        bVar1 = true;
      }
    } while ((!bVar1) && (iVar4 = iVar9 + 1, iVar9 + 1 < iVar5));
    DAT_451b_003e = piVar3;
    if (bVar1) {
      DAT_451b_003a = iVar9;
      FUN_54f6_0000(1);
      auto_str_type_the_new_message(0);
      DAT_4125_166a = 1;
      auto_str_join_data_not_found(0x18);
      FUN_540d_03ae();
    }
    else {
      FUN_1d28_050d(piVar2,uVar6);
    }
  }
  return;
}


