
void __cdecl16far FUN_8611_0629(int param_1,int param_2)

{
  if (param_1 < 5) {
    param_1 = 4;
  }
  if (param_2 < 5) {
    param_2 = 4;
  }
  if (0x6a < param_1) {
    param_1 = 0x6b;
  }
  if (0x96 < param_2) {
    param_2 = 0x97;
  }
  if (param_2 < 4) {
    DAT_3c04_0175 = 0;
  }
  else {
    DAT_3c04_0175 = param_2 + -4;
  }
  if (param_1 < 4) {
    DAT_3c04_0177 = 0;
  }
  else {
    DAT_3c04_0177 = param_1 + -4;
  }
  if (param_1 + 4 < 0x70) {
    DAT_3c04_017b = param_1 + 4;
  }
  else {
    DAT_3c04_017b = 0x6f;
  }
  if (param_2 + 4 < 0x9c) {
    DAT_3c04_0179 = param_2 + 4;
  }
  else {
    DAT_3c04_0179 = 0x9b;
  }
  DAT_3c04_017d = param_1;
  DAT_3c04_017f = param_2;
  FUN_8611_006c();
  return;
}


