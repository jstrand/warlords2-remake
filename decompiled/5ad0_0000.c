
undefined2 __cdecl16far ai_phase_move(void)

{
  byte *pbVar1;
  uint *puVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  int in_DX;
  int extraout_DX;
  undefined2 uVar6;
  
  iVar4 = army_count;
  while (iVar5 = iVar4 + -1, iVar4 != 0) {
    in_DX = 0x3c04;
    iVar4 = iVar5;
    if (((*(char *)(iVar5 * 0x16 + -0x777b) == (char)current_player) &&
        (*(int *)(iVar5 * 0x16 + -0x7780) != -1)) &&
       (puVar2 = (uint *)(iVar5 * 0x16 + -0x7774), *puVar2 = *puVar2 & 0xfdff,
       *(char *)(iVar5 * 0x16 + -0x7779) < '\x02')) {
      puVar2 = (uint *)(iVar5 * 0x16 + -0x7774);
      *puVar2 = *puVar2 | 0x200;
    }
  }
  while (iVar5 = in_DX, iVar4 = FUN_8c07_05c4(), iVar4 != 0 || iVar5 != 0) {
    *(uint *)(iVar4 + 0xc) = *(uint *)(iVar4 + 0xc) | 0x200;
    in_DX = iVar5;
    if ((-1 < *(int *)(iVar4 + 0x12)) && ('\x01' < *(char *)(iVar4 + 7))) {
      FUN_8c07_06eb();
      if (((((*(byte *)(iVar4 + 0xe) & 0xf) == 0) ||
           (((*(byte *)(iVar4 + 0xe) & 0xf) == 1 &&
            (in_DX = 0x3c04, city_count <= *(byte *)(iVar4 + 0xf) >> 1)))) ||
          (((*(byte *)(iVar4 + 0xe) & 0xf) == 1 &&
           ((uint)(*(byte *)(iVar4 + 0xf) >> 1) == *(uint *)((int)DAT_451b_018e + 0x46))))) ||
         (((*(byte *)(iVar4 + 0xe) & 0xf) == 1 &&
          (*(char *)(*(byte *)(*(int *)((uint)(*(byte *)(iVar4 + 0xf) >> 1) * 0x41 + 0x157f) * 0xe0
                              + *(int *)((uint)(*(byte *)(iVar4 + 0xf) >> 1) * 0x41 + 0x157d) * 2) +
                    0x710) != '\n')))) {
        iVar4 = 8;
        while (iVar5 = iVar4 + -1, iVar4 != 0) {
          iVar4 = iVar5;
          if (((undefined2 *)&DAT_451b_1ede)[iVar5 * 2] != 0 ||
              *(int *)((undefined *)&DAT_451b_1ee0 + iVar5 * 4) != 0) {
            uVar6 = (undefined2)
                    ((ulong)*(undefined4 *)((undefined2 *)&DAT_451b_1ede + iVar5 * 2) >> 0x10);
            pbVar1 = (byte *)((int)*(undefined4 *)((undefined2 *)&DAT_451b_1ede + iVar5 * 2) + 0xe);
            *pbVar1 = *pbVar1 & 0xf0;
            pbVar1 = (byte *)(((undefined2 *)&DAT_451b_1ede)[iVar5 * 2] + 0xf);
            *pbVar1 = *pbVar1 & 1;
            *(undefined2 *)(((undefined2 *)&DAT_451b_1ede)[iVar5 * 2] + 0x14) = 0xffff;
            *(undefined2 *)(((undefined2 *)&DAT_451b_1ede)[iVar5 * 2] + 0x12) = 0xffff;
            puVar2 = (uint *)(((undefined2 *)&DAT_451b_1ede)[iVar5 * 2] + 0xc);
            *puVar2 = *puVar2 | 0x40;
          }
        }
        in_DX = -1;
      }
      else if (((*(uint *)(iVar4 + 0xc) & 0x80) == 0) || (iVar3 = FUN_5ad0_05ff(), iVar3 != 0)) {
        if (((*(byte *)(iVar4 + 0xe) & 0xf) == 1) &&
           (*(char *)((uint)(*(byte *)(iVar4 + 0xf) >> 1) * 0x41 + 0x1592) == current_player)) {
          *(int *)((int)DAT_451b_1f02 + 0x12) =
               *(int *)((uint)(*(byte *)(iVar4 + 0xf) >> 1) * 0x41 + 0x157d) + 1;
          *(int *)((int)DAT_451b_1f02 + 0x14) =
               *(int *)((uint)(*(byte *)(iVar4 + 0xf) >> 1) * 0x41 + 0x157f) + 1;
        }
        uVar6 = (undefined2)((ulong)DAT_451b_1f02 >> 0x10);
        move_stack_to(0x8c07,*(undefined2 *)((int)DAT_451b_1f02 + 0x12),
                      *(undefined2 *)((int)DAT_451b_1f02 + 0x14),0);
        in_DX = extraout_DX;
      }
    }
  }
  return 1;
}


