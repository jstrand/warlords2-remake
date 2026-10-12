
void __cdecl16far hero_recruit(undefined4 param_1,int param_2)

{
  int *piVar1;
  int iVar2;
  int iVar3;
  undefined4 local_12;
  undefined2 local_e;
  int local_a;
  int local_8;
  int local_6;
  undefined2 local_4;
  
  local_4 = DAT_451b_058c;
  local_6 = 0;
  while ((local_6 < 0x28 && (*(int *)(local_6 * 2 + 0x543) != 0))) {
    local_6 = local_6 + 1;
  }
  sprintf((char *)CONCAT22(0x3c04,(char *)(local_6 * 0x14 + 0x223)),(char *)0x4125115a,param_1);
  *(undefined2 *)(local_6 * 2 + 0x543) = 1;
  *(int *)(local_6 * 2 + 0x593) = param_2;
  *(byte *)(local_6 + 0x5e3) = *(byte *)(local_6 + 0x5e3) & 0xfc;
  *(byte *)(local_6 + 0x5e3) = *(byte *)(local_6 + 0x5e3) & 3;
  FUN_6f8c_0bff(local_4,1,&local_8);
  piVar1 = (int *)FUN_1b62_0309();
  local_12 = (int *)CONCAT22(param_2,piVar1);
  *local_12 = local_8;
  piVar1[1] = local_a;
  DAT_451b_058a = local_8;
  DAT_451b_0588 = local_a;
  *(undefined1 *)(piVar1 + 2) = 0x1c;
  *(undefined1 *)((int)piVar1 + 7) = 0xe;
  *(undefined1 *)(piVar1 + 3) = 0xe;
  *(undefined1 *)(piVar1 + 4) = 5;
  *(undefined1 *)((int)piVar1 + 9) = (undefined1)local_4;
  *(undefined1 *)(piVar1 + 5) = (undefined1)local_6;
  *(undefined1 *)((int)piVar1 + 0xb) = 0;
  local_e = *(uint *)(local_a * 0xe0 + local_8 * 2) & 0xf0ff;
  local_e = CONCAT11(local_e._1_1_ | (byte)current_player & 0xf,(undefined1)local_e);
  *(undefined2 *)(local_a * 0xe0 + local_8 * 2) = local_e;
  DAT_451b_058e = local_12;
  FUN_1b62_02e2(local_12);
  iVar2 = game_turn();
  if (iVar2 < 2) {
    *(undefined1 *)(current_player * 0x1d + 0xcff) = 3;
    iVar2 = army_count;
    do {
      iVar3 = iVar2 + -1;
      if (iVar2 == 0) break;
      iVar2 = iVar3;
    } while ((param_2 != 0x2e1d) || ((int *)(iVar3 * 0x16 + -0x7780) != piVar1));
    *(int *)(current_player * 0x1d + 0xd00) = iVar3;
  }
  else {
    hero_brings_allies();
  }
  FUN_6d51_1244(current_player,0,DAT_451b_058c,0,param_1);
  return;
}


