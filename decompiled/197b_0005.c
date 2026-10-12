
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined2 __cdecl16far
auto_str_cannot_allocate_this_newbm(undefined4 param_1,char *param_2,char *param_3)

{
  undefined4 uVar1;
  int iVar2;
  undefined2 uVar3;
  undefined2 uVar4;
  int iVar5;
  uint unaff_DI;
  undefined2 unaff_SS;
  char *pcVar6;
  int iVar7;
  undefined2 uVar8;
  char local_70 [80];
  undefined2 local_20;
  undefined2 uStack_1e;
  undefined4 local_1c;
  undefined4 local_18;
  undefined4 local_14;
  undefined4 local_10;
  int local_c;
  undefined4 local_a;
  undefined4 local_6;
  
  strcpy((char *)CONCAT22(unaff_SS,local_70),param_2);
  pcVar6 = strcat((char *)CONCAT22(unaff_SS,local_70),param_3);
  uVar4 = (undefined2)((ulong)pcVar6 >> 0x10);
  DAT_451b_0cfa = open((char *)CONCAT22(unaff_SS,local_70),2);
  if (DAT_451b_0cfa != -1) {
    read(DAT_451b_0cfa,(void *)0x64125,unaff_DI);
    iVar7 = (int)param_1;
    uVar8 = (undefined2)((ulong)param_1 >> 0x10);
    iVar2 = FUN_2012_01fe(iVar7,uVar8,(int)_DAT_451b_0cf6,(int)((ulong)_DAT_451b_0cf6 >> 0x10));
    if (iVar2 != 0) {
      FUN_2012_07cc(iVar7,uVar8,0);
      if ((*(long *)(iVar7 + 10) != 0) && ((*(uint *)(iVar7 + 0x35) & 10) == 0)) {
        for (iVar2 = 0; iVar2 < (int)(uint)*(byte *)(iVar7 + 0x34); iVar2 = iVar2 + 1) {
          uVar1 = *(undefined4 *)(iVar7 + iVar2 * 4 + 10);
          uVar3 = mem_lock((int)uVar1,(int)((ulong)uVar1 >> 0x10));
          iVar5 = iVar7 + iVar2 * 4;
          *(undefined2 *)(iVar5 + 0x1c) = uVar4;
          *(undefined2 *)(iVar5 + 0x1a) = uVar3;
        }
      }
      local_20 = mem_alloc(0x800,0,0,0,0);
      uStack_1e = uVar4;
      iVar2 = mem_lock(local_20,uVar4);
      local_6 = CONCAT22(uVar4,iVar2);
      local_a = CONCAT22(uVar4,iVar2 + 0x400);
      local_c = (DAT_451b_0cf6 / 8) * DAT_451b_0cf8;
      local_10 = *(undefined4 *)(iVar7 + 0x1a);
      local_14 = *(undefined4 *)(iVar7 + 0x1e);
      local_18 = *(undefined4 *)(iVar7 + 0x22);
      local_1c = *(undefined4 *)(iVar7 + 0x26);
      FUN_1719_0059(local_10,DAT_451b_0cfa,local_6,local_a,0x400,local_c,4,local_14,(int)local_18,
                    (int)((ulong)local_18 >> 0x10),(int)local_1c,(int)((ulong)local_1c >> 0x10),0);
      FUN_1d28_048e(local_20,uStack_1e);
      close(DAT_451b_0cfa);
      if ((*(long *)(iVar7 + 10) != 0) && ((*(uint *)(iVar7 + 0x35) & 10) == 0)) {
        for (iVar2 = 0; iVar2 < (int)(uint)*(byte *)(iVar7 + 0x34); iVar2 = iVar2 + 1) {
          uVar1 = *(undefined4 *)(iVar7 + iVar2 * 4 + 10);
          mem_unlock((int)uVar1,(int)((ulong)uVar1 >> 0x10));
        }
      }
      return 1;
    }
    printf(s_Cannot_allocate_this_newbm_4125_1ef6);
  }
  return 0;
}


