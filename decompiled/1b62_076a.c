
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl16far FUN_1b62_076a(void)

{
  undefined2 *puVar1;
  int iVar2;
  undefined2 uVar3;
  
  if (DAT_4125_2bda == 0) {
    uVar3 = (undefined2)((ulong)DAT_451b_1f02 >> 0x10);
    puVar1 = (undefined2 *)DAT_451b_1f02;
    if (*(char *)(puVar1 + 2) == '\x1c') {
      hero_drop_items(puVar1,uVar3,*DAT_451b_1f02,puVar1[1]);
    }
    FUN_1b62_03be(DAT_451b_1f02);
  }
  else {
    for (iVar2 = 0; iVar2 < 8; iVar2 = iVar2 + 1) {
      if (((undefined2 *)&DAT_451b_1ede)[iVar2 * 2] != 0 ||
          *(int *)((undefined *)&DAT_451b_1ee0 + iVar2 * 4) != 0) {
        uVar3 = (undefined2)
                ((ulong)*(undefined4 *)((undefined2 *)&DAT_451b_1ede + iVar2 * 2) >> 0x10);
        if ((*(char *)((int)*(undefined4 *)((undefined2 *)&DAT_451b_1ede + iVar2 * 2) + 4) == '\x1c'
            ) && (hero_drop_items(*(undefined4 *)((undefined2 *)&DAT_451b_1ede + iVar2 * 2),
                                  *(undefined2 *)((undefined2 *)&DAT_451b_1ede)[iVar2 * 2],
                                  *(undefined2 *)(((undefined2 *)&DAT_451b_1ede)[iVar2 * 2] + 2)),
                 *(int *)(current_player * 0xc + 0x1103) != 0)) {
          if ((*(int *)((undefined *)&DAT_451b_1ee0 + iVar2 * 4) == 0x2e1d) &&
             (*(int *)(current_player * 0xc + 0x1107) * 0x16 + -0x7780 ==
              ((undefined2 *)&DAT_451b_1ede)[iVar2 * 2])) {
            *(undefined2 *)(current_player * 0xc + 0x1103) = 0;
          }
        }
        FUN_1b62_03be();
      }
    }
  }
  DAT_451b_1f02 = (undefined2 *)0x0;
  _DAT_451b_1efe = 0;
  DAT_4125_2bda = 0;
  refresh_map_owners();
  compute_upkeep();
  FUN_828e_0af3();
  game_screen_start(0);
  return;
}


