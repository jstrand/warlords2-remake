
undefined2 __cdecl16far FUN_8c07_06eb(int *param_1)

{
  uint *puVar1;
  char cVar2;
  char cVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  undefined2 uVar7;
  int iVar8;
  int iVar9;
  undefined2 in_DX;
  int iVar10;
  int *piVar11;
  int iVar12;
  int iVar13;
  int local_a;
  
  iVar12 = 0;
  cVar3 = -1;
  local_a = 0;
  iVar5 = FUN_1b62_0024(current_player);
  iVar6 = is_computer_turn();
  piVar11 = (int *)param_1;
  iVar13 = (int)((ulong)param_1 >> 0x10);
  if ((iVar6 == 0) && (iVar6 = auto_ui_armies_sunk(*param_1,piVar11[1]), iVar6 != 0)) {
    uVar7 = 0;
  }
  else {
    cVar2 = *(char *)((int)piVar11 + 0x11);
    iVar6 = *param_1;
    iVar4 = piVar11[1];
    iVar9 = 8;
    while (iVar10 = iVar9 + -1, iVar8 = army_count, iVar9 != 0) {
      *(undefined2 *)((undefined *)&DAT_451b_1ee0 + iVar10 * 4) = 0;
      ((undefined2 *)&DAT_451b_1ede)[iVar10 * 2] = 0;
      iVar9 = iVar10;
    }
    while (iVar9 = iVar8 + -1, iVar8 != 0) {
      iVar8 = iVar9;
      if ((((*(int *)(iVar9 * 0x16 + -0x7780) == iVar6) &&
           (*(int *)(iVar9 * 0x16 + -0x777e) == iVar4)) &&
          (*(char *)(iVar9 * 0x16 + -0x777b) == (char)current_player)) &&
         (((*(char *)(iVar9 * 0x16 + -0x776f) == cVar2 && (cVar2 != '\0')) ||
          ((iVar13 == 0x2e1d && ((int *)(iVar9 * 0x16 + -0x7780) == piVar11)))))) {
        puVar1 = (uint *)(iVar9 * 0x16 + -0x7774);
        *puVar1 = *puVar1 | 0x200;
        *(undefined2 *)((undefined *)&DAT_451b_1ee0 + iVar12 * 4) = 0x2e1d;
        ((undefined2 *)&DAT_451b_1ede)[iVar12 * 2] = iVar9 * 0x16 + -0x7780;
        iVar12 = iVar12 + 1;
        if (cVar3 < *(char *)(iVar5 + *(char *)(iVar9 * 0x16 + -0x777c))) {
          cVar3 = *(char *)(iVar5 + *(char *)(iVar9 * 0x16 + -0x777c));
          local_a = iVar9;
        }
      }
    }
    DAT_451b_1f02 = (undefined2 *)CONCAT22(0x2e1d,(undefined2 *)(local_a * 0x16 + -0x7780));
    stack_movement_mode();
    FUN_1b62_09ce();
    FUN_8961_07f0();
    FUN_828e_0af3();
    FUN_8c07_0955(1);
    if ((*(int *)(current_player * 2 + 0xd0) == 0) ||
       ((DAT_3c04_0159 != 0 &&
        ((DAT_4125_3254 == 0 ||
         (iVar5 = FUN_8611_1055(*DAT_451b_1f02,((undefined2 *)DAT_451b_1f02)[1],1),
         *(char *)(iVar5 + 0x2d56) != '\x0e')))))) {
      FUN_8611_05b4();
    }
    uVar7 = 1;
  }
  return uVar7;
}


