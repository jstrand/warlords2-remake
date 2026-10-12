
clock_t * __cdecl16far clock(clock_t *__return_storage_ptr__)

{
  code *pcVar1;
  char cVar2;
  int extraout_DX;
  undefined2 local_6;
  
  pcVar1 = (code *)swi(0x1a);
  cVar2 = (*pcVar1)();
  DAT_451b_31f6 = DAT_451b_31f6 + cVar2;
  if (cVar2 != '\0') {
    DAT_0000_0470 = 1;
    pcVar1 = (code *)swi(0x21);
    (*pcVar1)();
  }
  local_6 = extraout_DX;
  if (DAT_451b_31f6 != '\0') {
    local_6 = extraout_DX + 0xb0;
  }
  return (clock_t *)(local_6 - DAT_4125_0081);
}


