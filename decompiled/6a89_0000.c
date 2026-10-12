
int __cdecl16far combat_terrain_class(int x,int y)

{
  int iVar1;
  uint uVar2;
  
  if ((*(byte *)(y * 0xe0 + x * 2 + 1) & 0x20) != 0) {
    return 0;
  }
  uVar2 = (int)*(char *)(*(byte *)(y * 0xe0 + x * 2) + 0x710) - 4;
  if (uVar2 < 8) {
                    /* WARNING: Switch is manually overridden */
    switch((undefined4)*(undefined2 *)(uVar2 * 2 + 0x7b)) {
    case 0x6a8fa:
      return 0;
    case 0x6a8fc:
      iVar1 = 2;
      break;
    case 0x6a901:
      iVar1 = 3;
      break;
    case 0x6a906:
      goto override_jmp_6a89_0065_case_3;
    }
  }
  else {
override_jmp_6a89_0065_case_3:
    iVar1 = 1;
  }
  return iVar1;
}


