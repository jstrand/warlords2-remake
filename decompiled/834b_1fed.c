
void __cdecl16far FUN_834b_1fed(undefined2 param_1,undefined2 param_2,undefined2 param_3)

{
  undefined2 uVar1;
  int iVar2;
  int iVar3;
  undefined2 uVar4;
  int local_12;
  int local_10;
  int local_e;
  int local_c;
  int local_a;
  int local_8;
  int local_6;
  int local_4;
  
  if ((opt_hidden_map != 0) &&
     (((*(int *)(current_player * 2 + 0xd0) == 0 || (DAT_3c04_0159 != 0)) && (DAT_4125_3254 == 0))))
  {
    uVar4 = 0x216d;
    FUN_216d_002e(&local_12);
    if (local_12 < 0) {
      local_e = local_e + local_12;
      local_12 = 0;
    }
    if (local_10 < 0) {
      local_c = local_c + local_10;
      local_10 = 0;
    }
    if (0x6f < local_12 + local_e) {
      local_e = 0x6f - local_12;
    }
    if (0x9b < local_10 + local_c) {
      local_c = 0x9b - local_10;
    }
    local_a = local_12 << 1;
    local_8 = local_10 << 1;
    local_6 = local_e << 1;
    local_4 = local_c << 1;
    iVar3 = local_a;
    if ((DAT_451b_158d != 0) && ((DAT_451b_15b8 & 10) == 0)) {
      for (iVar2 = 0; iVar3 = local_a, iVar2 < (int)(uint)DAT_451b_15b7; iVar2 = iVar2 + 1) {
        uVar4 = 0x1d28;
        uVar1 = mem_lock(((undefined4 *)&DAT_451b_158d)[iVar2]);
        ((undefined2 *)&DAT_451b_159f)[iVar2 * 2] = param_1;
        ((undefined2 *)&DAT_451b_159d)[iVar2 * 2] = uVar1;
      }
    }
    for (; iVar2 = local_8, iVar3 <= local_a + local_6; iVar3 = iVar3 + 1) {
      for (; iVar2 <= local_8 + local_4; iVar2 = iVar2 + 1) {
        (*DAT_451b_2a70)(uVar4,0x54e3,0x4125,iVar3,iVar2,0);
      }
    }
    if ((DAT_451b_158d != 0) && ((DAT_451b_15b8 & 10) == 0)) {
      for (iVar3 = 0; iVar3 < (int)(uint)DAT_451b_15b7; iVar3 = iVar3 + 1) {
        mem_unlock(((undefined4 *)&DAT_451b_158d)[iVar3]);
      }
    }
    iVar3 = FUN_54f6_08bf();
    if (iVar3 == 0) {
      FUN_8961_05ec();
      FUN_834b_0000(param_3,1,&local_12);
    }
  }
  return;
}


