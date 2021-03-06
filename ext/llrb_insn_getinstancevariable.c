#include "cruby.h"

static VALUE __attribute__ ((noinline))
vm_getivar_else(VALUE obj, ID id, IC ic)
{
  st_data_t index;
  struct st_table *iv_index_tbl = ROBJECT_IV_INDEX_TBL(obj);

  VALUE val = Qundef;
  if (iv_index_tbl) {
    if (st_lookup(iv_index_tbl, id, &index)) {
      if (index < ROBJECT_NUMIV(obj)) {
        val = ROBJECT_IVPTR(obj)[index];
      }
      ic->ic_value.index = index;
      ic->ic_serial = RCLASS_SERIAL(RBASIC(obj)->klass);
    }
  }
  if (UNLIKELY(val == Qundef)) {
    if (RTEST(ruby_verbose))
      rb_warning("instance variable %"PRIsVALUE" not initialized", QUOTE_ID(id));
    val = Qnil;
  }
  return val;
}

static inline VALUE
vm_getivar(VALUE obj, ID id, IC ic)
{
  if (LIKELY(RB_TYPE_P(obj, T_OBJECT))) {
    VALUE val = Qundef;
    if (LIKELY(ic->ic_serial == RCLASS_SERIAL(RBASIC(obj)->klass))) {
      st_index_t index = ic->ic_value.index;
      if (LIKELY(index < ROBJECT_NUMIV(obj))) {
        val = ROBJECT_IVPTR(obj)[index];
      }
      if (UNLIKELY(val == Qundef)) {
        if (RTEST(ruby_verbose))
          rb_warning("instance variable %"PRIsVALUE" not initialized", QUOTE_ID(id));
        val = Qnil;
      }
      return val;
    }
    else {
      vm_getivar_else(obj, id, ic);
    }
  }
  return rb_ivar_get(obj, id);
}

VALUE
llrb_insn_getinstancevariable(VALUE obj, ID id, VALUE ic_v)
{
  return vm_getivar(obj, id, (IC)ic_v);
}
