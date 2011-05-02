
#line 1 "tsv_parser.rl"
#include "tsv_parser.h"
#include <assert.h>

#define MARK(M, FPC) (ctx->M = FPC - data)
#define MARK_LEN(M, FPC) (FPC - (ctx->M + data))
#define MARK_PTR(M) (ctx->M + data)


#line 60 "tsv_parser.rl"



#line 16 "tsv_parser.c"
static const int tsv_start = 5;
static const int tsv_first_final = 5;
static const int tsv_error = 0;

static const int tsv_en_main = 5;


#line 63 "tsv_parser.rl"

void 
tsv_init(tsv_ctx *ctx, void* user)
{

	int cs = 0;
	
#line 32 "tsv_parser.c"
	{
	cs = tsv_start;
	}

#line 70 "tsv_parser.rl"
	ctx->cs = cs;
	ctx->user = user;
}

size_t 
tsv_execute(tsv_ctx *ctx, const char* data, size_t len, size_t off)
{
    const char *p, *pe, *eof;
    int cs = ctx->cs;

    assert(off <= len && "offset past end of buffer");

    p = data + off;
    pe = data + len;
	eof = pe;

    assert(*pe == '\0' && "pointer does not end on NUL");
    assert(pe - p == len - off && "pointers aren't same distance");

	
#line 58 "tsv_parser.c"
	{
	if ( p == pe )
		goto _test_eof;
	switch ( cs )
	{
tr16:
#line 24 "tsv_parser.rl"
	{
		if(ctx->on_record){
            ctx->on_record(ctx->user, MARK_PTR(key_start), ctx->key_len, MARK_PTR(value_start), MARK_LEN(value_start, p));
        }
    }
	goto st5;
tr20:
#line 30 "tsv_parser.rl"
	{
        //printf("integer \n");
    }
#line 24 "tsv_parser.rl"
	{
		if(ctx->on_record){
            ctx->on_record(ctx->user, MARK_PTR(key_start), ctx->key_len, MARK_PTR(value_start), MARK_LEN(value_start, p));
        }
    }
	goto st5;
tr25:
#line 34 "tsv_parser.rl"
	{
        //printf("double \n");
    }
#line 24 "tsv_parser.rl"
	{
		if(ctx->on_record){
            ctx->on_record(ctx->user, MARK_PTR(key_start), ctx->key_len, MARK_PTR(value_start), MARK_LEN(value_start, p));
        }
    }
	goto st5;
st5:
	if ( ++p == pe )
		goto _test_eof5;
case 5:
#line 100 "tsv_parser.c"
	switch( (*p) ) {
		case 10: goto tr2;
		case 13: goto tr2;
		case 32: goto tr2;
	}
	goto tr13;
tr13:
#line 11 "tsv_parser.rl"
	{
        MARK(key_start, p);
    }
	goto st1;
st1:
	if ( ++p == pe )
		goto _test_eof1;
case 1:
#line 117 "tsv_parser.c"
	switch( (*p) ) {
		case 9: goto tr1;
		case 10: goto tr2;
		case 13: goto tr2;
		case 32: goto tr3;
	}
	goto st1;
tr1:
#line 15 "tsv_parser.rl"
	{
        ctx->key_len = MARK_LEN(key_start, p);
    }
	goto st2;
st2:
	if ( ++p == pe )
		goto _test_eof2;
case 2:
#line 135 "tsv_parser.c"
	switch( (*p) ) {
		case 9: goto tr5;
		case 10: goto tr2;
		case 13: goto tr2;
		case 32: goto tr6;
	}
	if ( 48 <= (*p) && (*p) <= 57 )
		goto tr7;
	goto tr4;
tr4:
#line 19 "tsv_parser.rl"
	{
        MARK(value_start, p);
        //printf("mark value\n");
    }
	goto st6;
st6:
	if ( ++p == pe )
		goto _test_eof6;
case 6:
#line 156 "tsv_parser.c"
	switch( (*p) ) {
		case 9: goto tr15;
		case 10: goto tr16;
		case 13: goto tr17;
		case 32: goto tr18;
	}
	goto st6;
tr15:
#line 15 "tsv_parser.rl"
	{
        ctx->key_len = MARK_LEN(key_start, p);
    }
	goto st7;
tr5:
#line 15 "tsv_parser.rl"
	{
        ctx->key_len = MARK_LEN(key_start, p);
    }
#line 19 "tsv_parser.rl"
	{
        MARK(value_start, p);
        //printf("mark value\n");
    }
	goto st7;
st7:
	if ( ++p == pe )
		goto _test_eof7;
case 7:
#line 185 "tsv_parser.c"
	switch( (*p) ) {
		case 9: goto tr5;
		case 10: goto tr16;
		case 13: goto tr17;
		case 32: goto tr6;
	}
	if ( 48 <= (*p) && (*p) <= 57 )
		goto tr7;
	goto tr4;
tr17:
#line 24 "tsv_parser.rl"
	{
		if(ctx->on_record){
            ctx->on_record(ctx->user, MARK_PTR(key_start), ctx->key_len, MARK_PTR(value_start), MARK_LEN(value_start, p));
        }
    }
	goto st3;
tr21:
#line 30 "tsv_parser.rl"
	{
        //printf("integer \n");
    }
#line 24 "tsv_parser.rl"
	{
		if(ctx->on_record){
            ctx->on_record(ctx->user, MARK_PTR(key_start), ctx->key_len, MARK_PTR(value_start), MARK_LEN(value_start, p));
        }
    }
	goto st3;
tr26:
#line 34 "tsv_parser.rl"
	{
        //printf("double \n");
    }
#line 24 "tsv_parser.rl"
	{
		if(ctx->on_record){
            ctx->on_record(ctx->user, MARK_PTR(key_start), ctx->key_len, MARK_PTR(value_start), MARK_LEN(value_start, p));
        }
    }
	goto st3;
st3:
	if ( ++p == pe )
		goto _test_eof3;
case 3:
#line 231 "tsv_parser.c"
	if ( (*p) == 10 )
		goto st5;
	goto st0;
tr2:
#line 38 "tsv_parser.rl"
	{
        ctx->error = 1;
        printf("error \n");
        {p++; cs = 0; goto _out;}
    }
	goto st0;
#line 243 "tsv_parser.c"
st0:
cs = 0;
	goto _out;
tr18:
#line 15 "tsv_parser.rl"
	{
        ctx->key_len = MARK_LEN(key_start, p);
    }
	goto st8;
tr11:
#line 19 "tsv_parser.rl"
	{
        MARK(value_start, p);
        //printf("mark value\n");
    }
	goto st8;
tr6:
#line 15 "tsv_parser.rl"
	{
        ctx->key_len = MARK_LEN(key_start, p);
    }
#line 19 "tsv_parser.rl"
	{
        MARK(value_start, p);
        //printf("mark value\n");
    }
	goto st8;
st8:
	if ( ++p == pe )
		goto _test_eof8;
case 8:
#line 275 "tsv_parser.c"
	switch( (*p) ) {
		case 9: goto tr11;
		case 10: goto tr16;
		case 13: goto tr17;
		case 32: goto tr11;
	}
	if ( 48 <= (*p) && (*p) <= 57 )
		goto tr12;
	goto tr10;
tr10:
#line 19 "tsv_parser.rl"
	{
        MARK(value_start, p);
        //printf("mark value\n");
    }
	goto st9;
st9:
	if ( ++p == pe )
		goto _test_eof9;
case 9:
#line 296 "tsv_parser.c"
	switch( (*p) ) {
		case 10: goto tr16;
		case 13: goto tr17;
	}
	goto st9;
tr12:
#line 19 "tsv_parser.rl"
	{
        MARK(value_start, p);
        //printf("mark value\n");
    }
	goto st10;
st10:
	if ( ++p == pe )
		goto _test_eof10;
case 10:
#line 313 "tsv_parser.c"
	switch( (*p) ) {
		case 10: goto tr20;
		case 13: goto tr21;
		case 46: goto st11;
	}
	if ( 48 <= (*p) && (*p) <= 57 )
		goto st10;
	goto st9;
st11:
	if ( ++p == pe )
		goto _test_eof11;
case 11:
	switch( (*p) ) {
		case 10: goto tr16;
		case 13: goto tr17;
	}
	if ( 48 <= (*p) && (*p) <= 57 )
		goto st12;
	goto st9;
st12:
	if ( ++p == pe )
		goto _test_eof12;
case 12:
	switch( (*p) ) {
		case 10: goto tr25;
		case 13: goto tr26;
	}
	if ( 48 <= (*p) && (*p) <= 57 )
		goto st12;
	goto st9;
tr7:
#line 19 "tsv_parser.rl"
	{
        MARK(value_start, p);
        //printf("mark value\n");
    }
	goto st13;
st13:
	if ( ++p == pe )
		goto _test_eof13;
case 13:
#line 355 "tsv_parser.c"
	switch( (*p) ) {
		case 9: goto tr15;
		case 10: goto tr20;
		case 13: goto tr21;
		case 32: goto tr18;
		case 46: goto st14;
	}
	if ( 48 <= (*p) && (*p) <= 57 )
		goto st13;
	goto st6;
st14:
	if ( ++p == pe )
		goto _test_eof14;
case 14:
	switch( (*p) ) {
		case 9: goto tr15;
		case 10: goto tr16;
		case 13: goto tr17;
		case 32: goto tr18;
	}
	if ( 48 <= (*p) && (*p) <= 57 )
		goto st15;
	goto st6;
st15:
	if ( ++p == pe )
		goto _test_eof15;
case 15:
	switch( (*p) ) {
		case 9: goto tr15;
		case 10: goto tr25;
		case 13: goto tr26;
		case 32: goto tr18;
	}
	if ( 48 <= (*p) && (*p) <= 57 )
		goto st15;
	goto st6;
tr3:
#line 15 "tsv_parser.rl"
	{
        ctx->key_len = MARK_LEN(key_start, p);
    }
	goto st4;
st4:
	if ( ++p == pe )
		goto _test_eof4;
case 4:
#line 402 "tsv_parser.c"
	switch( (*p) ) {
		case 9: goto tr11;
		case 10: goto tr2;
		case 13: goto tr2;
		case 32: goto tr11;
	}
	if ( 48 <= (*p) && (*p) <= 57 )
		goto tr12;
	goto tr10;
	}
	_test_eof5: cs = 5; goto _test_eof; 
	_test_eof1: cs = 1; goto _test_eof; 
	_test_eof2: cs = 2; goto _test_eof; 
	_test_eof6: cs = 6; goto _test_eof; 
	_test_eof7: cs = 7; goto _test_eof; 
	_test_eof3: cs = 3; goto _test_eof; 
	_test_eof8: cs = 8; goto _test_eof; 
	_test_eof9: cs = 9; goto _test_eof; 
	_test_eof10: cs = 10; goto _test_eof; 
	_test_eof11: cs = 11; goto _test_eof; 
	_test_eof12: cs = 12; goto _test_eof; 
	_test_eof13: cs = 13; goto _test_eof; 
	_test_eof14: cs = 14; goto _test_eof; 
	_test_eof15: cs = 15; goto _test_eof; 
	_test_eof4: cs = 4; goto _test_eof; 

	_test_eof: {}
	if ( p == eof )
	{
	switch ( cs ) {
	case 6: 
	case 7: 
	case 8: 
	case 9: 
	case 11: 
	case 14: 
#line 24 "tsv_parser.rl"
	{
		if(ctx->on_record){
            ctx->on_record(ctx->user, MARK_PTR(key_start), ctx->key_len, MARK_PTR(value_start), MARK_LEN(value_start, p));
        }
    }
	break;
	case 1: 
	case 2: 
	case 4: 
#line 38 "tsv_parser.rl"
	{
        ctx->error = 1;
        printf("error \n");
        {p++; cs = 0; goto _out;}
    }
	break;
	case 10: 
	case 13: 
#line 30 "tsv_parser.rl"
	{
        //printf("integer \n");
    }
#line 24 "tsv_parser.rl"
	{
		if(ctx->on_record){
            ctx->on_record(ctx->user, MARK_PTR(key_start), ctx->key_len, MARK_PTR(value_start), MARK_LEN(value_start, p));
        }
    }
	break;
	case 12: 
	case 15: 
#line 34 "tsv_parser.rl"
	{
        //printf("double \n");
    }
#line 24 "tsv_parser.rl"
	{
		if(ctx->on_record){
            ctx->on_record(ctx->user, MARK_PTR(key_start), ctx->key_len, MARK_PTR(value_start), MARK_LEN(value_start, p));
        }
    }
	break;
#line 482 "tsv_parser.c"
	}
	}

	_out: {}
	}

#line 90 "tsv_parser.rl"

    ctx->cs = cs;
    ctx->nread += p - (data + off);

    assert(p <= pe && "buffer overflow after parsing execute");
    assert(ctx->nread <= len && "nread longer than length");
    //assert(ctx->pos <= len && "body starts after buffer end");

    return(ctx->nread);
}



