
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl16far FUN_22bf_0224(int param_1,int param_2,int param_3,int param_4)

{
  undefined2 uVar1;
  
  uVar1 = (undefined2)((ulong)DAT_4125_34e7 >> 0x10);
  FUN_1ec0_0036((int)(0x1400 / (ulong)(uint)(*(int *)((int)DAT_4125_34e7 + 4) * _DAT_451b_2a54)),
                *(uint *)((int)DAT_4125_34e7 + 6) / 0x14);
  uVar1 = (undefined2)((ulong)DAT_4125_34e7 >> 0x10);
  FUN_1ec0_0067(_DAT_451b_2a54 * param_3,_DAT_451b_2a54 * param_4,
                _DAT_451b_2a54 * ((*(int *)((int)DAT_4125_34e7 + 4) - param_1) + param_3),
                _DAT_451b_2a54 * ((*(int *)((int)DAT_4125_34e7 + 6) - param_2) + param_4));
  return;
}


