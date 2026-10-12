
void __text_write(undefined2 param_1,undefined2 param_2,undefined2 param_3,undefined2 param_4,
                 undefined2 param_5)

{
  if ((DAT_4125_3e7b == '\0') && (DAT_4125_3e81 != 0)) {
    __vram_copy(param_1,param_2,param_3,param_4,param_5);
  }
  else {
    __bios_text_write(param_1,param_2,param_3,param_4,param_5);
  }
  return;
}


