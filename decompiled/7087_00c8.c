
void __cdecl16far ui_choose_production(int slot)

{
  if (-1 < *(char *)(DAT_4125_0e98 * 0x41 + slot + 0x1593)) {
    DAT_4125_0e9e = (int)*(char *)(DAT_4125_0e98 * 0x41 + slot + 0x1593);
  }
  city_start_chosen_production();
  return;
}


