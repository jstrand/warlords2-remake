
undefined2 __cdecl16far
auto_str_plane_handle_not_allocated(undefined4 param_1,int param_2,int param_3,uint param_4)

{
  int iVar1;
  uint uVar2;
  long lVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  undefined2 uVar7;
  
  uVar7 = (undefined2)((ulong)param_1 >> 0x10);
  iVar4 = (int)param_1;
  if (((*(int *)(iVar4 + 8) != param_2) || (*(int *)(iVar4 + 6) != param_3)) ||
     (*(byte *)(iVar4 + 0x34) != param_4)) {
    uVar2 = 0;
    *(uint *)(iVar4 + 0x35) = *(uint *)(iVar4 + 0x35) & 0xfffd;
    iVar1 = FUN_2012_0975(iVar4,uVar7);
    if (iVar1 == 0) {
      return 0;
    }
    if (((DAT_4125_3384 != 0) && ((*(uint *)(iVar4 + 0x35) & 4) != 0)) &&
       (uVar2 = FUN_1ecc_005c(param_2 * param_3), uVar2 != 0)) {
      *(uint *)(iVar4 + 0x35) = *(uint *)(iVar4 + 0x35) | 2;
      for (iVar1 = 0; iVar1 < (int)param_4; iVar1 = iVar1 + 1) {
        *(long *)(iVar4 + iVar1 * 4 + 0x1a) = (ulong)uVar2 << 0x10;
      }
    }
    if (uVar2 == 0) {
      for (iVar1 = 0; iVar1 < (int)param_4; iVar1 = iVar1 + 1) {
        lVar3 = (long)param_3;
        iVar5 = mem_alloc(param_2 * lVar3 * 2,0,iVar1 + 100,iVar1 + 100 >> 0xf);
        iVar6 = iVar4 + iVar1 * 4;
        *(int *)(iVar6 + 0xc) = (int)lVar3;
        *(int *)(iVar6 + 10) = iVar5;
        if (iVar5 == 0 && (int)lVar3 == 0) {
          FUN_2012_0975(iVar4,uVar7);
          return 0;
        }
      }
      for (iVar1 = 0; iVar1 < (int)param_4; iVar1 = iVar1 + 1) {
        iVar5 = iVar4 + iVar1 * 4;
        if (*(int *)(iVar5 + 10) == 0 && *(int *)(iVar5 + 0xc) == 0) {
          printf(s_Plane_handle__d_not_allocated_4125_350f,iVar1);
          getch();
          return 0;
        }
      }
    }
    *(undefined1 *)(iVar4 + 0x34) = (undefined1)param_4;
    *(undefined4 *)(iVar4 + 0x2a) = 0;
    if ((*(uint *)((int)DAT_4125_34e7 + 0xe) == param_4) || (param_4 == 1)) {
      FUN_216d_002e(iVar4,uVar7,0,0,param_2 << (DAT_451b_2a40 & 0x1f),param_3);
    }
    *(int *)(iVar4 + 8) = param_2;
    *(undefined4 *)(iVar4 + 0x2e) = 0;
    *(undefined1 *)(iVar4 + 0x32) = 0;
    *(undefined1 *)(iVar4 + 0x33) = 0;
    *(undefined2 *)(iVar4 + 0x35) = 0;
    *(undefined4 *)(iVar4 + 0x37) = 0;
  }
  return 1;
}


