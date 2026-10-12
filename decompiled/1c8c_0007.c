
void __cdecl16far FUN_1c8c_0007(int param_1,int param_2)

{
  int iVar1;
  int iVar2;
  int iVar3;
  undefined2 uVar4;
  undefined1 local_e0 [16];
  undefined1 local_d0 [200];
  int local_8;
  int local_6;
  int local_4;
  
  if (DAT_451b_1f02 != (int *)0x0) {
    FUN_22bf_0af4(&local_6);
    if (param_1 == 0) {
      iVar2 = (local_6 + -400) / 2;
      iVar3 = (local_4 + -0x1e) / 2;
    }
    else {
      iVar2 = DAT_3c04_0177 + (local_6 + -0x10) / 0x28;
      iVar3 = DAT_3c04_0175 + (local_4 + -0x1e) / 0x28;
    }
    iVar1 = FUN_8611_15ff(1,iVar2,iVar3);
    if (iVar1 != 0) {
      if ((*DAT_451b_1f02 == iVar2) && (((int *)DAT_451b_1f02)[1] == iVar3)) {
        for (iVar2 = 0; iVar2 < 8; iVar2 = iVar2 + 1) {
          if (((undefined2 *)&DAT_451b_1ede)[iVar2 * 2] != 0 ||
              *(int *)((undefined *)&DAT_451b_1ee0 + iVar2 * 4) != 0) {
            uVar4 = (undefined2)
                    ((ulong)*(undefined4 *)((undefined2 *)&DAT_451b_1ede + iVar2 * 2) >> 0x10);
            *(undefined2 *)((int)*(undefined4 *)((undefined2 *)&DAT_451b_1ede + iVar2 * 2) + 0x12) =
                 0xffff;
            *(undefined2 *)(((undefined2 *)&DAT_451b_1ede)[iVar2 * 2] + 0x14) = 0xffff;
          }
        }
        FUN_8611_006c();
      }
      else {
        for (iVar1 = 0; iVar1 < 8; iVar1 = iVar1 + 1) {
          if (((undefined2 *)&DAT_451b_1ede)[iVar1 * 2] != 0 ||
              *(int *)((undefined *)&DAT_451b_1ee0 + iVar1 * 4) != 0) {
            uVar4 = (undefined2)
                    ((ulong)*(undefined4 *)((undefined2 *)&DAT_451b_1ede + iVar1 * 2) >> 0x10);
            *(int *)((int)*(undefined4 *)((undefined2 *)&DAT_451b_1ede + iVar1 * 2) + 0x12) = iVar2;
            *(int *)(((undefined2 *)&DAT_451b_1ede)[iVar1 * 2] + 0x14) = iVar3;
          }
        }
        FUN_1555_1868(local_e0);
        iVar2 = pathfind(local_e0);
        if (iVar2 == 0) {
          sound_chord();
        }
        else {
          FUN_1c8c_0963(*DAT_451b_1f02,((int *)DAT_451b_1f02)[1],local_d0);
          local_8 = DAT_451b_1ea0;
          if (DAT_4125_2bda == 0) {
            local_8 = (int)*(char *)((int)(int *)DAT_451b_1f02 + 7);
          }
          DAT_4125_2eaa = path_step_costs(local_e0);
          FUN_8611_006c();
          if (param_2 != 0) {
            FUN_8065_0174();
          }
        }
      }
    }
  }
  return;
}


