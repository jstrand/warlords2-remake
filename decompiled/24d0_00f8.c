
void __cdecl16far
FUN_24d0_00f8(undefined2 param_1,undefined2 param_2,undefined2 *param_3,int param_4,int param_5,
             undefined4 param_6,undefined2 param_7,undefined2 param_8)

{
  undefined2 *puVar1;
  uint uVar2;
  undefined2 uVar3;
  undefined2 uVar4;
  
  FUN_22bf_07a5();
  uVar3 = (undefined2)((ulong)DAT_451b_30e9 >> 0x10);
  uVar2 = param_4 + *(int *)((int)DAT_451b_30e9 + 8);
  param_5 = param_5 + *(int *)((int)DAT_451b_30e9 + 10);
  puVar1 = (undefined2 *)param_3;
  uVar3 = (undefined2)((ulong)param_3 >> 0x10);
  if (DAT_4125_3384 == 0) {
    FUN_2012_01fe(0x3708,0x4125,puVar1[2] + 0x1e,puVar1[3]);
    FUN_2012_07cc(0x3708,0x4125,0);
    (*DAT_451b_2a74)(0x2012,param_1,param_2,*param_3,puVar1[1],puVar1[2],puVar1[3],0x3708,0x4125,
                     uVar2 & 0xf,0);
    uVar4 = (undefined2)((ulong)DAT_451b_30e9 >> 0x10);
    (*DAT_451b_2a68)(0x2012,0x3708,0x4125,0,0,puVar1[2] + 0x1e,puVar1[3],
                     *(undefined2 *)((int)DAT_451b_30e9 + 4),*(undefined2 *)((int)DAT_451b_30e9 + 6)
                     ,uVar2 & 0xfff0,param_5,0,0,0,0);
    uVar3 = 0x3708;
  }
  else {
    FUN_2012_01fe(0x3708,0x4125,puVar1[2] + 0x1e,puVar1[3]);
    FUN_2012_07cc(0x3708,0x4125,0);
    (*DAT_451b_2a74)(0x2012,param_1,param_2,*param_3,puVar1[1],puVar1[2],puVar1[3],0x3708,0x4125,
                     uVar2 & 0xf,0);
    uVar4 = (undefined2)((ulong)param_6 >> 0x10);
    FUN_2012_0232(0x3743,0x4125,puVar1[2] + 0x1e,puVar1[3],*(undefined1 *)((int)param_6 + 0x34));
    FUN_2012_07cc(0x3743,0x4125,0xf);
    (*DAT_451b_2a74)(0x2012,(int)param_6,uVar4,param_7,param_8,puVar1[2],puVar1[3],0x3743,0x4125,
                     uVar2 & 0xf,0);
    uVar4 = (undefined2)((ulong)DAT_451b_30e9 >> 0x10);
    (*DAT_451b_2a68)(0x2012,0x3708,0x4125,0,0,puVar1[2] + 0x1e,puVar1[3],
                     *(undefined2 *)((int)DAT_451b_30e9 + 4),*(undefined2 *)((int)DAT_451b_30e9 + 6)
                     ,uVar2 & 0xfff0,param_5,0x3743,0x4125,0,0);
    FUN_2012_0975(0x3708,0x4125);
    uVar3 = 0x3743;
  }
  FUN_2012_0975(uVar3,0x4125);
  FUN_22bf_0757();
  return;
}


