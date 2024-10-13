#include <input.h>
#include <Arduino.h>
#include <hardware.h>

struct input_directions _old_presses;

void input_init()
{
  pinMode(JOY_BU, INPUT_PULLUP);
  pinMode(JOY_BR, INPUT_PULLUP);
  pinMode(JOY_BB, INPUT_PULLUP);
  pinMode(JOY_BL, INPUT_PULLUP);
  pinMode(JOY_BP, INPUT_PULLUP);
  _old_presses={0};
}

bool input_get(struct input_directions *inputs)
{
  /* This can be faster when masking the entire GPIO instead */
  bool changed=false;
  inputs->up=!digitalReadFast(JOY_BB);
  inputs->right=!digitalReadFast(JOY_BR);
  inputs->down=!digitalReadFast(JOY_BU);
  inputs->left=!digitalReadFast(JOY_BL);
  inputs->pressed=!digitalReadFast(JOY_BP);

  /* Check for changes */
  /* Take the first 4 bytes as a 32 int to check at once */
  uint32_t first4_old = *(uint32_t*)&_old_presses;
  uint32_t first4_new = *(uint32_t*)inputs;
  if(first4_old!=first4_new)
  {
    changed = true;
    memcpy(&_old_presses, inputs, 4);
  }else{
    /* The first didnt match, maybe the last byte does */
    if(_old_presses.pressed!=inputs->pressed)
    {
      changed = true;
      _old_presses.pressed=inputs->pressed;
    }
  }

  inputs->debug=*(uint32_t*)inputs;
  return changed;
}