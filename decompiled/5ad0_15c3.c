
undefined2 __cdecl16far FUN_5ad0_15c3(int param_1,int param_2)

{
  uint *puVar1;
  int iVar2;
  int iVar3;
  undefined2 uVar4;
  int iVar5;
  int iVar6;
  
  iVar2 = *(int *)(param_2 * 0x1f + 0x811);
  iVar3 = *(int *)(param_2 * 0x1f + 0x813);
  if (((*(int *)(param_1 * 0x16 + -0x7780) == iVar2) &&
      (*(int *)(param_1 * 0x16 + -0x777e) == iVar3)) &&
     (*(char *)(param_1 * 0x16 + -0x7779) != '\0')) {
    if ((*(char *)(*(byte *)(iVar3 * 0xe0 + iVar2 * 2) + 0x710) == '\v') &&
       ((*(char *)(param_2 * 0x1f + 0x829) == '\x01' ||
        ((*(byte *)(iVar3 * 0xe0 + iVar2 * 2 + 1) & 0x40) == 0)))) {
      DAT_451b_1f02._0_2_ = param_1 * 0x16 + -0x7780;
      DAT_451b_1f02._2_2_ = 0x2e1d;
      site_search();
      if ((*(char *)(param_2 * 0x1f + 0x829) == '\x01') &&
         ((opt_quests != 0 && (*(int *)(current_player * 0xc + 0x1103) == 0)))) {
        quest_assign(1);
      }
      if (*(char *)(param_1 * 0x16 + -0x777c) != '\x1c') goto LAB_5ad0_1631;
      for (iVar5 = 0; iVar5 < DAT_451b_1e9e; iVar5 = iVar5 + 1) {
        if (((undefined2 *)&DAT_451b_1ede)[iVar5 * 2] != 0 ||
            *(int *)((undefined *)&DAT_451b_1ee0 + iVar5 * 4) != 0) {
          puVar1 = (uint *)((int)*(undefined4 *)((undefined2 *)&DAT_451b_1ede + iVar5 * 2) + 0xc);
          *puVar1 = *puVar1 & 0xffbf;
        }
      }
      if ((opt_hidden_map != 0) && (iVar5 = army_count, *(char *)(param_2 * 0x1f + 0x829) == '\x05')
         ) {
        while (iVar6 = iVar5 + -1, iVar5 != 0) {
          iVar5 = iVar6;
          if ((((*(char *)(iVar6 * 0x16 + -0x777b) == (char)current_player) &&
               ((*(byte *)(iVar6 * 0x16 + -0x7772) & 0xf) == 0)) &&
              (((undefined1 *)&DAT_451b_0668)[*(char *)(iVar6 * 0x16 + -0x777c) * 6] != '\0')) &&
             ((*(int *)(iVar6 * 0x16 + -0x7780) == iVar2 &&
              (*(int *)(iVar6 * 0x16 + -0x777e) == iVar3)))) {
            puVar1 = (uint *)(iVar6 * 0x16 + -0x7774);
            *puVar1 = *puVar1 | 0x20;
            FUN_57ea_0e4b(iVar6,0xffff);
          }
        }
      }
    }
    uVar4 = 2;
  }
  else {
LAB_5ad0_1631:
    uVar4 = 0;
  }
  return uVar4;
}


