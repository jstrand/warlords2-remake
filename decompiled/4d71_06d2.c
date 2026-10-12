
void __cdecl16far random_map_draw_ridges(void)

{
  int iVar1;
  int iVar2;
  int iVar3;
  undefined2 uVar4;
  
  if (1 < *(int *)((int)DAT_451b_002a + 0x611e)) {
    iVar3 = 0;
    while( true ) {
      uVar4 = (undefined2)((ulong)DAT_451b_002a >> 0x10);
      iVar2 = (int)DAT_451b_002a;
      if (*(int *)(iVar2 + 0x611e) <= iVar3) break;
      if (*(int *)((int)(undefined2 *)&side_upkeep + iVar2 + iVar3 * 0xe) == 1) {
        if (*(int *)(iVar2 + iVar3 * 0xe + 0x5ddc) == 0) {
          FUN_4d71_08f3(*(undefined4 *)(iVar2 + iVar3 * 0xe + 0x5dd6));
        }
        else {
          iVar2 = 0;
          while( true ) {
            uVar4 = (undefined2)((ulong)DAT_451b_002a >> 0x10);
            iVar1 = (int)DAT_451b_002a;
            if (*(int *)(iVar1 + iVar3 * 0xe + 0x5ddc) <= iVar2) break;
            if (*(int *)((int)(undefined2 *)&side_upkeep +
                        iVar1 + *(int *)(iVar1 + iVar3 * 0xe + iVar2 * 2 + 0x5dde) * 0xe) == 1) {
              FUN_4d71_098c(*(undefined4 *)(iVar1 + iVar3 * 0xe + 0x5dd6),
                            *(undefined4 *)
                             (iVar1 + *(int *)(iVar1 + iVar3 * 0xe + iVar2 * 2 + 0x5dde) * 0xe +
                             0x5dd6));
            }
            else {
              FUN_4d71_0962(*(undefined4 *)(iVar1 + iVar3 * 0xe + 0x5dd6),
                            *(undefined4 *)
                             (iVar1 + *(int *)(iVar1 + iVar3 * 0xe + iVar2 * 2 + 0x5dde) * 0xe +
                             0x5dd6));
            }
            iVar2 = iVar2 + 1;
          }
        }
      }
      else if (*(int *)(iVar2 + iVar3 * 0xe + 0x5ddc) == 0) {
        FUN_4d71_0884(*(undefined4 *)(iVar2 + iVar3 * 0xe + 0x5dd6));
      }
      else {
        iVar2 = 0;
        while( true ) {
          uVar4 = (undefined2)((ulong)DAT_451b_002a >> 0x10);
          iVar1 = (int)DAT_451b_002a;
          if (*(int *)(iVar1 + iVar3 * 0xe + 0x5ddc) <= iVar2) break;
          FUN_4d71_0962(*(undefined4 *)(iVar1 + iVar3 * 0xe + 0x5dd6),
                        *(undefined4 *)
                         (iVar1 + *(int *)(iVar1 + iVar3 * 0xe + iVar2 * 2 + 0x5dde) * 0xe + 0x5dd6)
                       );
          iVar2 = iVar2 + 1;
        }
      }
      iVar3 = iVar3 + 1;
    }
  }
  return;
}


