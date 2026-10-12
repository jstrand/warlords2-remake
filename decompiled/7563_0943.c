
void __cdecl16far FUN_7563_0943(int *param_1,int param_2)

{
  int iVar1;
  undefined2 uVar2;
  
  uVar2 = (undefined2)((ulong)param_1 >> 0x10);
  if (-1 < param_2) {
    if (*(char *)(*(byte *)(((int *)param_1)[1] * 0xe0 + *param_1 * 2) + 0x710) == '\x02') {
      *(undefined1 *)(param_2 * 0x1d + 0xcff) = 0;
      iVar1 = is_computer_turn();
      if (iVar1 == 0) {
        sound_splash();
      }
    }
    else {
      *(undefined1 *)(param_2 * 0x1d + 0xcff) = 1;
      *(undefined2 *)(param_2 * 0x1d + 0xd00) = 0;
      *(int *)(param_2 * 0x1d + 0xd02) = *param_1;
      *(int *)(param_2 * 0x1d + 0xd04) = ((int *)param_1)[1];
    }
  }
  return;
}


