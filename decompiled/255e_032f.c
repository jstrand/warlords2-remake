
/* WARNING: Removing unreachable block (ram,0x000259ac) */

undefined2 __cdecl16far music_load_xmi(undefined2 param_1,undefined2 param_2)

{
  undefined2 *puVar1;
  undefined2 *puVar2;
  void *buf;
  int iVar3;
  undefined2 *puVar4;
  uint unaff_DI;
  undefined2 *puVar5;
  undefined2 unaff_SS;
  long lVar6;
  char local_58 [80];
  undefined2 local_8;
  undefined4 local_6;
  
  if (DAT_4125_37a3 == '\0') {
    return 0;
  }
  puVar5 = &local_8;
  puVar4 = (undefined2 *)&DAT_4125_3872;
  for (iVar3 = 1; iVar3 != 0; iVar3 = iVar3 + -1) {
    puVar2 = puVar5;
    puVar5 = puVar5 + 1;
    puVar1 = puVar4;
    puVar4 = puVar4 + 1;
    *puVar2 = *puVar1;
  }
  if (DAT_4125_37ac != '\0') {
    puVar5 = &local_8;
    puVar4 = (undefined2 *)&DAT_4125_3874;
    for (iVar3 = 1; iVar3 != 0; iVar3 = iVar3 + -1) {
      puVar2 = puVar5;
      puVar5 = puVar5 + 1;
      puVar1 = puVar4;
      puVar4 = puVar4 + 1;
      *puVar2 = *puVar1;
    }
  }
  if (DAT_4125_37ad != '\0') {
    puVar5 = &local_8;
    puVar4 = (undefined2 *)&DAT_4125_3876;
    for (iVar3 = 1; iVar3 != 0; iVar3 = iVar3 + -1) {
      puVar2 = puVar5;
      puVar5 = puVar5 + 1;
      puVar1 = puVar4;
      puVar4 = puVar4 + 1;
      *puVar2 = *puVar1;
    }
  }
  sprintf((char *)CONCAT22(unaff_SS,local_58),(char *)0x41253878,param_1,param_2,&local_8);
  iVar3 = open((char *)CONCAT22(unaff_SS,local_58),2);
  if (-1 < iVar3) {
    lVar6 = filelength(iVar3);
    if (10000 < lVar6) {
      lVar6 = 9999;
    }
    local_6._2_2_ = (undefined2)((ulong)lVar6 >> 0x10);
    local_6._0_2_ = (undefined2)lVar6;
    DAT_451b_30fa = local_6._2_2_;
    DAT_451b_30f8 = (undefined2)local_6;
    buf = (void *)CONCAT22((undefined2)local_6,DAT_451b_30f2);
    local_6 = lVar6;
    read(iVar3,buf,unaff_DI);
    close(iVar3);
    return 1;
  }
  return 0;
}


