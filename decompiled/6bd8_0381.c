
void __cdecl16far produce_garrison(int city)

{
  byte *pbVar1;
  int iVar2;
  undefined2 in_DX;
  
  iVar2 = city_produce_army(city,*(undefined2 *)(city * 0x41 + 0x157d),
                            *(undefined2 *)(city * 0x41 + 0x157f),0xffff);
  *(undefined1 *)(iVar2 + 7) = 0;
  *(undefined1 *)(city * 0x41 + 0x15a9) = 0xff;
  *(undefined1 *)(city * 0x41 + 0x15aa) = 0;
  pbVar1 = (byte *)(*(int *)(city * 0x41 + 0x157f) * 0xe0 + *(int *)(city * 0x41 + 0x157d) * 2 + 1);
  *pbVar1 = *pbVar1 | 0x10;
  return;
}


