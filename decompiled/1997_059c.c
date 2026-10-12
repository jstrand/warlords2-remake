
void __cdecl16far FUN_1997_059c(int param_1)

{
  ((undefined1 *)&DAT_4125_1f56)[param_1 * 0xc] =
       ((undefined1 *)&DAT_4125_1f56)[param_1 * 0xc] & 0xfb;
  ((undefined1 *)&DAT_4125_1f56)[param_1 * 0xc] =
       ((undefined1 *)&DAT_4125_1f56)[param_1 * 0xc] & 0xbf;
  FUN_2012_0975(*(undefined4 *)(param_1 * 0xc + 0x1f57));
  if ((((undefined1 *)&DAT_4125_1f56)[param_1 * 0xc] & 0x20) != 0) {
    FUN_2012_0975(*(undefined4 *)(param_1 * 0xc + 0x1f5c));
  }
  return;
}


