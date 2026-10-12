
void __cdecl16far
FUN_1f00_03a1(undefined4 param_1,uint param_2,int param_3,uint param_4,undefined2 param_5,
             undefined4 param_6,uint param_7,int param_8)

{
  bool bVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  undefined2 in_DX;
  int extraout_DX;
  int extraout_DX_00;
  int extraout_DX_01;
  int iVar5;
  int iVar6;
  int iVar7;
  undefined2 uVar8;
  undefined2 uVar9;
  undefined2 uVar10;
  undefined2 uVar11;
  bool bVar12;
  int local_14;
  int local_c;
  
  uVar8 = (undefined2)((ulong)param_1 >> 0x10);
  iVar5 = (int)param_1;
  uVar9 = (undefined2)((ulong)DAT_4125_34e7 >> 0x10);
  if ((*(uint *)(iVar5 + 4) < *(uint *)((int)DAT_4125_34e7 + 4)) ||
     (*(uint *)(iVar5 + 6) < *(uint *)((int)DAT_4125_34e7 + 6))) {
    bVar1 = false;
  }
  else {
    bVar1 = true;
  }
  if (((!bVar1) && (*(long *)(iVar5 + 10) != 0)) && ((*(uint *)(iVar5 + 0x35) & 10) == 0)) {
    for (local_14 = 0; local_14 < (int)(uint)*(byte *)(iVar5 + 0x34); local_14 = local_14 + 1) {
      iVar2 = iVar5 + local_14 * 4;
      uVar9 = mem_lock(*(undefined2 *)(iVar2 + 10),*(undefined2 *)(iVar2 + 0xc));
      iVar2 = iVar5 + local_14 * 4;
      *(undefined2 *)(iVar2 + 0x1c) = in_DX;
      *(undefined2 *)(iVar2 + 0x1a) = uVar9;
    }
  }
  if ((((param_2 & 7) == 0) && ((param_4 & 7) == 0)) && ((param_7 & 7) == 0)) {
    iVar7 = (int)param_4 >> (DAT_451b_2a3e & 0x1f);
    iVar2 = iVar7 / 4;
    bVar12 = iVar2 == (iVar7 + 3) / 4;
    if ((*(long *)(iVar5 + 0x1a) == *(long *)(iVar5 + 0x1e)) &&
       (*(long *)((int)param_6 + 0x1a) == *(long *)((int)param_6 + 0x1e))) {
      FUN_1e68_0047(1);
      FUN_1e68_003a();
      if (((*(uint *)(iVar5 + 0x35) & 1) == 0) || (param_3 < *(int *)(iVar5 + 6))) {
        uVar9 = *(undefined2 *)(iVar5 + 0x1c);
        iVar2 = *(int *)(iVar5 + 0x1a) + param_3 * *(int *)(iVar5 + 8) * 2 +
                ((int)param_2 >> (DAT_451b_2a3e & 0x1f));
      }
      else {
        uVar10 = (undefined2)((ulong)*(undefined4 *)(iVar5 + 0x37) >> 0x10);
        iVar2 = (int)*(undefined4 *)(iVar5 + 0x37);
        uVar9 = *(undefined2 *)(iVar2 + 0x1c);
        iVar2 = *(int *)(iVar2 + 0x1a) +
                (param_3 - *(int *)(iVar5 + 6)) * *(int *)((int)*(undefined4 *)(iVar5 + 0x37) + 8) *
                2 + ((int)param_2 >> (DAT_451b_2a3e & 0x1f));
      }
      if (((*(uint *)((int)param_6 + 0x35) & 1) == 0) || (param_8 < *(int *)((int)param_6 + 6))) {
        uVar10 = *(undefined2 *)((int)param_6 + 0x1c);
        iVar6 = *(int *)((int)param_6 + 0x1a) + param_8 * *(int *)((int)param_6 + 8) * 2 +
                ((int)param_7 >> (DAT_451b_2a3e & 0x1f));
      }
      else {
        uVar11 = (undefined2)((ulong)*(undefined4 *)((int)param_6 + 0x37) >> 0x10);
        iVar6 = (int)*(undefined4 *)((int)param_6 + 0x37);
        uVar10 = *(undefined2 *)(iVar6 + 0x1c);
        iVar6 = *(int *)(iVar6 + 0x1a) +
                (param_8 - *(int *)((int)param_6 + 6)) *
                *(int *)((int)*(undefined4 *)((int)param_6 + 0x37) + 8) * 2 +
                ((int)param_7 >> (DAT_451b_2a3e & 0x1f));
      }
      (*DAT_451b_2a64)(0x1e68,iVar2,uVar9,iVar6,uVar10,param_5,iVar7,*(int *)(iVar5 + 8) * 2 - iVar7
                       ,*(int *)((int)param_6 + 8) * 2 - iVar7);
      FUN_1e68_0047(0);
    }
    else {
      for (local_c = 0; local_c < (int)(uint)*(byte *)(iVar5 + 0x34); local_c = local_c + 1) {
        if (((bVar1) &&
            (iVar6 = iVar5 + local_c * 4, *(int *)(iVar6 + 10) != 0 || *(int *)(iVar6 + 0xc) != 0))
           && ((*(uint *)(iVar5 + 0x35) & 10) == 0)) {
          iVar6 = iVar5 + local_c * 4;
          uVar9 = mem_lock(*(undefined2 *)(iVar6 + 10),*(undefined2 *)(iVar6 + 0xc));
          iVar6 = iVar5 + local_c * 4;
          *(int *)(iVar6 + 0x1c) = iVar2;
          *(undefined2 *)(iVar6 + 0x1a) = uVar9;
        }
        FUN_1e68_0018(local_c);
        if (((*(uint *)(iVar5 + 0x35) & 1) == 0) || (param_3 < *(int *)(iVar5 + 6))) {
          iVar2 = iVar5 + local_c * 4;
          uVar9 = *(undefined2 *)(iVar2 + 0x1c);
          iVar2 = *(int *)(iVar2 + 0x1a) + param_3 * *(int *)(iVar5 + 8) * 2 +
                  ((int)param_2 >> (DAT_451b_2a3e & 0x1f));
        }
        else {
          uVar10 = (undefined2)((ulong)*(undefined4 *)(iVar5 + 0x37) >> 0x10);
          iVar2 = (int)*(undefined4 *)(iVar5 + 0x37) + local_c * 4;
          uVar9 = *(undefined2 *)(iVar2 + 0x1c);
          iVar2 = *(int *)(iVar2 + 0x1a) +
                  (param_3 - *(int *)(iVar5 + 6)) * *(int *)((int)*(undefined4 *)(iVar5 + 0x37) + 8)
                  * 2 + ((int)param_2 >> (DAT_451b_2a3e & 0x1f));
        }
        if (((*(uint *)((int)param_6 + 0x35) & 1) == 0) || (param_8 < *(int *)((int)param_6 + 6))) {
          iVar6 = (int)param_6 + local_c * 4;
          uVar10 = *(undefined2 *)(iVar6 + 0x1c);
          iVar6 = *(int *)(iVar6 + 0x1a) + param_8 * *(int *)((int)param_6 + 8) * 2 +
                  ((int)param_7 >> (DAT_451b_2a3e & 0x1f));
        }
        else {
          uVar11 = (undefined2)((ulong)*(undefined4 *)((int)param_6 + 0x37) >> 0x10);
          iVar6 = (int)*(undefined4 *)((int)param_6 + 0x37) + local_c * 4;
          uVar10 = *(undefined2 *)(iVar6 + 0x1c);
          iVar6 = *(int *)(iVar6 + 0x1a) +
                  (param_8 - *(int *)((int)param_6 + 6)) *
                  *(int *)((int)*(undefined4 *)((int)param_6 + 0x37) + 8) * 2 +
                  ((int)param_7 >> (DAT_451b_2a3e & 0x1f));
        }
        iVar3 = *(int *)(iVar5 + 8) * 2 - iVar7;
        iVar4 = *(int *)((int)param_6 + 8) * 2 - iVar7;
        if (bVar12) {
          (*DAT_451b_2a5c)(0x1e68,iVar2,uVar9,iVar6,uVar10,param_5,iVar7,iVar3,iVar4);
          iVar2 = extraout_DX;
        }
        else if (iVar7 / 2 == (iVar7 + 1) / 2) {
          (*DAT_451b_2a60)(0x1e68,iVar2,uVar9,iVar6,uVar10,param_5,iVar7,iVar3,iVar4);
          iVar2 = extraout_DX_00;
        }
        else {
          (*DAT_451b_2a64)(0x1e68,iVar2,uVar9,iVar6,uVar10,param_5,iVar7,iVar3,iVar4);
          iVar2 = extraout_DX_01;
        }
        iVar6 = iVar5 + local_c * 4;
        if ((*(int *)(iVar6 + 10) != 0 || *(int *)(iVar6 + 0xc) != 0) &&
           ((*(uint *)(iVar5 + 0x35) & 10) == 0)) {
          iVar6 = iVar5 + local_c * 4;
          mem_unlock(*(undefined2 *)(iVar6 + 10),*(undefined2 *)(iVar6 + 0xc));
        }
      }
    }
  }
  else {
    FUN_1f00_0884(iVar5,uVar8,param_2,param_3,param_4,param_5,(int)param_6,param_6._2_2_,param_7,
                  param_8);
  }
  return;
}


