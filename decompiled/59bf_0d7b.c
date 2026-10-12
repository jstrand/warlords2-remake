
void __cdecl16far FUN_59bf_0d7b(int param_1,int param_2)

{
  int iVar1;
  int iVar2;
  undefined2 uVar3;
  undefined2 unaff_SS;
  undefined1 local_6a [8];
  undefined2 local_62;
  int local_60;
  undefined1 local_5c [29];
  undefined2 local_3f;
  undefined2 local_3d;
  undefined2 local_3b;
  undefined2 local_39;
  undefined2 local_25;
  undefined2 local_23;
  undefined2 local_21;
  undefined2 local_1f;
  undefined2 local_1d;
  undefined2 local_1b;
  undefined2 local_19;
  undefined2 local_17;
  int local_15;
  int local_13;
  int local_11;
  int local_f;
  undefined2 local_b;
  undefined2 local_9;
  int local_7;
  undefined2 local_5;
  
  iVar1 = (int)DAT_451b_018e;
  uVar3 = (undefined2)((ulong)DAT_451b_018e >> 0x10);
  if (param_1 == 0) {
    *(undefined2 *)(iVar1 + 10) = 0x1e;
    *(undefined2 *)(iVar1 + 0x48) = 1;
    iVar1 = dice(1,4,0);
    *(int *)((int)DAT_451b_018e + 0x18) = iVar1;
    iVar1 = dice(1,4,0);
    *(int *)((int)DAT_451b_018e + 0x1e) = iVar1;
    iVar1 = dice(1,4,0);
    uVar3 = (undefined2)((ulong)DAT_451b_018e >> 0x10);
    iVar2 = (int)DAT_451b_018e;
    *(int *)(iVar2 + 0x1a) = iVar1;
    *(undefined2 *)(iVar2 + 0x24a) = 1;
    *(undefined2 *)(iVar2 + 0x38) = 0;
    *(undefined2 *)(iVar2 + 0x10) = 3;
    *(undefined2 *)(iVar2 + 0x12) = 3;
    *(undefined2 *)(iVar2 + 0x26) = 0;
    *(undefined2 *)(iVar2 + 0x28) = 0;
    *(undefined2 *)(iVar2 + 0x2a) = 0;
    *(undefined2 *)(iVar2 + 0x2c) = 0;
    *(undefined2 *)(iVar2 + 0x2e) = 0;
    *(undefined2 *)(iVar2 + 0x30) = 0;
    *(undefined2 *)(iVar2 + 0x32) = 0;
    *(undefined2 *)(iVar2 + 0x34) = 0;
    *(undefined2 *)(iVar2 + 0x36) = 0;
    *(undefined2 *)(iVar2 + 0x42) = 0;
    *(undefined2 *)(iVar2 + 0x44) = 0x50;
  }
  else {
    if (param_1 == 1) {
      *(undefined2 *)(iVar1 + 10) = 0x14;
      *(undefined2 *)(iVar1 + 0x48) = 0;
      iVar1 = dice(1,4,0);
      *(int *)((int)DAT_451b_018e + 0x18) = iVar1;
      iVar1 = dice(1,4,0);
      *(int *)((int)DAT_451b_018e + 0x1e) = iVar1;
      iVar1 = dice(1,8,0);
      uVar3 = (undefined2)((ulong)DAT_451b_018e >> 0x10);
      iVar2 = (int)DAT_451b_018e;
      *(int *)(iVar2 + 0x1c) = iVar1;
      *(uint *)(iVar2 + 0xc) = *(uint *)(iVar2 + 0xc) | 1;
      *(undefined2 *)(iVar2 + 0x38) = 0;
      *(undefined2 *)(iVar2 + 0x24a) = 2;
      *(undefined2 *)(iVar2 + 0x10) = 6;
      *(undefined2 *)(iVar2 + 0x12) = 0x12;
      *(undefined2 *)(iVar2 + 0x26) = 5;
      *(undefined2 *)(iVar2 + 0x28) = 10;
      *(undefined2 *)(iVar2 + 0x2a) = 0x14;
      *(undefined2 *)(iVar2 + 0x2c) = 1;
      *(undefined2 *)(iVar2 + 0x2e) = 5;
      *(undefined2 *)(iVar2 + 0x34) = 0;
      *(undefined2 *)(iVar2 + 0x32) = 0;
      *(undefined2 *)(iVar2 + 0x30) = 0;
      *(undefined2 *)(iVar2 + 0x36) = 0;
      *(undefined2 *)(iVar2 + 0x42) = 0;
    }
    else {
      if (param_1 != 2) goto LAB_59bf_0fa4;
      *(undefined2 *)(iVar1 + 10) = 10;
      *(undefined2 *)(iVar1 + 0x48) = 0;
      iVar1 = dice(1,10,0);
      *(int *)((int)DAT_451b_018e + 0x18) = iVar1;
      iVar1 = dice(1,8,0);
      *(int *)((int)DAT_451b_018e + 0x1a) = iVar1;
      iVar1 = dice(1,6,0);
      uVar3 = (undefined2)((ulong)DAT_451b_018e >> 0x10);
      iVar2 = (int)DAT_451b_018e;
      *(int *)(iVar2 + 0x1c) = iVar1;
      *(undefined2 *)(iVar2 + 0x24a) = 4;
      *(undefined2 *)(iVar2 + 0x38) = 1;
      *(uint *)(iVar2 + 0xc) = *(uint *)(iVar2 + 0xc) | 1;
      *(undefined2 *)(iVar2 + 0x10) = 7;
      *(undefined2 *)(iVar2 + 0x12) = 0;
      *(undefined2 *)(iVar2 + 0x26) = 5;
      *(undefined2 *)(iVar2 + 0x28) = 10;
      *(undefined2 *)(iVar2 + 0x2a) = 0x14;
      *(undefined2 *)(iVar2 + 0x2c) = 5;
      *(undefined2 *)(iVar2 + 0x2e) = 0x32;
      *(undefined2 *)(iVar2 + 0x34) = 0;
      *(undefined2 *)(iVar2 + 0x32) = 0;
      *(undefined2 *)(iVar2 + 0x30) = 0;
      *(undefined2 *)(iVar2 + 0x36) = 0x32;
      *(undefined2 *)(iVar2 + 0x42) = 1;
    }
    *(undefined2 *)((int)DAT_451b_018e + 0x44) = 0x23;
  }
LAB_59bf_0fa4:
  if ((*(int *)(param_2 * 2 + 0x137) != 0) && (*(int *)(param_2 * 2 + 0xd0) == 1)) {
    FUN_7715_006a(param_2,local_6a);
    iVar1 = dice(1,local_15,0);
    *(int *)((int)DAT_451b_018e + 0x18) = iVar1;
    iVar1 = dice(1,local_11,0);
    *(int *)((int)DAT_451b_018e + 0x1a) = iVar1;
    iVar1 = dice(1,local_13,0);
    *(int *)((int)DAT_451b_018e + 0x1c) = iVar1;
    iVar1 = dice(1,local_f,0);
    uVar3 = (undefined2)((ulong)DAT_451b_018e >> 0x10);
    iVar2 = (int)DAT_451b_018e;
    *(int *)(iVar2 + 0x1e) = iVar1;
    *(undefined2 *)(iVar2 + 0x24a) = local_62;
    *(undefined2 *)(iVar2 + 0x38) = local_5;
    if (local_60 != 0) {
      *(uint *)(iVar2 + 0xc) = *(uint *)(iVar2 + 0xc) | 1;
    }
    uVar3 = (undefined2)((ulong)DAT_451b_018e >> 0x10);
    iVar1 = (int)DAT_451b_018e;
    *(undefined2 *)(iVar1 + 0x48) = local_3f;
    *(undefined2 *)(iVar1 + 0x10) = local_3d;
    *(undefined2 *)(iVar1 + 0x12) = local_3b;
    *(undefined2 *)(iVar1 + 10) = local_39;
    *(undefined2 *)(iVar1 + 0x26) = local_25;
    *(undefined2 *)(iVar1 + 0x28) = local_23;
    *(undefined2 *)(iVar1 + 0x2a) = local_21;
    *(undefined2 *)(iVar1 + 0x2c) = local_1f;
    *(undefined2 *)(iVar1 + 0x2e) = local_1d;
    *(undefined2 *)(iVar1 + 0x30) = local_17;
    *(undefined2 *)(iVar1 + 0x32) = local_19;
    *(undefined2 *)(iVar1 + 0x34) = local_1b;
    *(undefined2 *)(iVar1 + 0x36) = local_b;
    *(undefined2 *)(iVar1 + 0x42) = local_9;
    iVar1 = dice(1,10,0);
    *(int *)((int)DAT_451b_018e + 0x44) = local_7 * 10 + iVar1;
    for (iVar1 = 0; iVar1 < 0x1d; iVar1 = iVar1 + 1) {
      *(undefined1 *)(param_2 * 0x1d + iVar1 + 0x60b) = local_5c[iVar1];
    }
  }
  return;
}


