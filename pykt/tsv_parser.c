
#line 1 "tsv_parser.rl"
#include "tsv_parser.h"
#include <assert.h>

#define MARK(M, FPC) (ctx->M = FPC - data)
#define MARK_LEN(M, FPC) (FPC - (ctx->M + data))
#define MARK_PTR(M) (ctx->M + data)


#line 60 "tsv_parser.rl"



#line 16 "tsv_parser.c"
static const int tsv_start = 4;
static const int tsv_first_final = 4;
static const int tsv_error = 0;

static const int tsv_en_main = 4;


#line 63 "tsv_parser.rl"

inline void 
tsv_init(tsv_ctx *ctx)
{

	int cs = 0;
	
#line 32 "tsv_parser.c"
	{
	cs = tsv_start;
	}

#line 70 "tsv_parser.rl"
	ctx->cs = cs;
}

inline size_t 
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

	
#line 57 "tsv_parser.c"
	{
	if ( p == pe )
		goto _test_eof;
	switch ( cs )
	{
tr10:
#line 24 "tsv_parser.rl"
	{
		if(ctx->on_record){
            ctx->on_record(ctx, MARK_PTR(key_start), ctx->key_len, MARK_PTR(value_start), MARK_LEN(value_start, p));
        }
    }
	goto st4;
tr12:
#line 30 "tsv_parser.rl"
	{
        //printf("integer \n");
    }
#line 24 "tsv_parser.rl"
	{
		if(ctx->on_record){
            ctx->on_record(ctx, MARK_PTR(key_start), ctx->key_len, MARK_PTR(value_start), MARK_LEN(value_start, p));
        }
    }
	goto st4;
tr17:
#line 34 "tsv_parser.rl"
	{
        //printf("double \n");
    }
#line 24 "tsv_parser.rl"
	{
		if(ctx->on_record){
            ctx->on_record(ctx, MARK_PTR(key_start), ctx->key_len, MARK_PTR(value_start), MARK_LEN(value_start, p));
        }
    }
	goto st4;
st4:
	if ( ++p == pe )
		goto _test_eof4;
case 4:
#line 99 "tsv_parser.c"
	switch( (*p) ) {
		case 13: goto tr2;
		case 32: goto tr2;
	}
	if ( 9 <= (*p) && (*p) <= 10 )
		goto tr2;
	goto tr8;
tr8:
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
		case 32: goto tr1;
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
		case 9: goto tr4;
		case 10: goto tr2;
		case 13: goto tr2;
		case 32: goto tr4;
	}
	if ( 48 <= (*p) && (*p) <= 57 )
		goto tr5;
	goto tr3;
tr3:
#line 19 "tsv_parser.rl"
	{
        MARK(value_start, p);
        //printf("mark value\n");
    }
	goto st5;
st5:
	if ( ++p == pe )
		goto _test_eof5;
case 5:
#line 156 "tsv_parser.c"
	switch( (*p) ) {
		case 10: goto tr10;
		case 13: goto tr11;
	}
	goto st5;
tr11:
#line 24 "tsv_parser.rl"
	{
		if(ctx->on_record){
            ctx->on_record(ctx, MARK_PTR(key_start), ctx->key_len, MARK_PTR(value_start), MARK_LEN(value_start, p));
        }
    }
	goto st3;
tr13:
#line 30 "tsv_parser.rl"
	{
        //printf("integer \n");
    }
#line 24 "tsv_parser.rl"
	{
		if(ctx->on_record){
            ctx->on_record(ctx, MARK_PTR(key_start), ctx->key_len, MARK_PTR(value_start), MARK_LEN(value_start, p));
        }
    }
	goto st3;
tr18:
#line 34 "tsv_parser.rl"
	{
        //printf("double \n");
    }
#line 24 "tsv_parser.rl"
	{
		if(ctx->on_record){
            ctx->on_record(ctx, MARK_PTR(key_start), ctx->key_len, MARK_PTR(value_start), MARK_LEN(value_start, p));
        }
    }
	goto st3;
st3:
	if ( ++p == pe )
		goto _test_eof3;
case 3:
#line 198 "tsv_parser.c"
	if ( (*p) == 10 )
		goto st4;
	goto st0;
tr2:
#line 38 "tsv_parser.rl"
	{
        ctx->error = 1;
        //printf("error \n");
        {p++; cs = 0; goto _out;}
    }
	goto st0;
#line 210 "tsv_parser.c"
st0:
cs = 0;
	goto _out;
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
#line 225 "tsv_parser.c"
	switch( (*p) ) {
		case 9: goto tr4;
		case 10: goto tr10;
		case 13: goto tr11;
		case 32: goto tr4;
	}
	if ( 48 <= (*p) && (*p) <= 57 )
		goto tr5;
	goto tr3;
tr5:
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
#line 246 "tsv_parser.c"
	switch( (*p) ) {
		case 10: goto tr12;
		case 13: goto tr13;
		case 46: goto st8;
	}
	if ( 48 <= (*p) && (*p) <= 57 )
		goto st7;
	goto st5;
st8:
	if ( ++p == pe )
		goto _test_eof8;
case 8:
	switch( (*p) ) {
		case 10: goto tr10;
		case 13: goto tr11;
	}
	if ( 48 <= (*p) && (*p) <= 57 )
		goto st9;
	goto st5;
st9:
	if ( ++p == pe )
		goto _test_eof9;
case 9:
	switch( (*p) ) {
		case 10: goto tr17;
		case 13: goto tr18;
	}
	if ( 48 <= (*p) && (*p) <= 57 )
		goto st9;
	goto st5;
	}
	_test_eof4: cs = 4; goto _test_eof; 
	_test_eof1: cs = 1; goto _test_eof; 
	_test_eof2: cs = 2; goto _test_eof; 
	_test_eof5: cs = 5; goto _test_eof; 
	_test_eof3: cs = 3; goto _test_eof; 
	_test_eof6: cs = 6; goto _test_eof; 
	_test_eof7: cs = 7; goto _test_eof; 
	_test_eof8: cs = 8; goto _test_eof; 
	_test_eof9: cs = 9; goto _test_eof; 

	_test_eof: {}
	if ( p == eof )
	{
	switch ( cs ) {
	case 5: 
	case 6: 
	case 8: 
#line 24 "tsv_parser.rl"
	{
		if(ctx->on_record){
            ctx->on_record(ctx, MARK_PTR(key_start), ctx->key_len, MARK_PTR(value_start), MARK_LEN(value_start, p));
        }
    }
	break;
	case 1: 
	case 2: 
#line 38 "tsv_parser.rl"
	{
        ctx->error = 1;
        //printf("error \n");
        {p++; cs = 0; goto _out;}
    }
	break;
	case 7: 
#line 30 "tsv_parser.rl"
	{
        //printf("integer \n");
    }
#line 24 "tsv_parser.rl"
	{
		if(ctx->on_record){
            ctx->on_record(ctx, MARK_PTR(key_start), ctx->key_len, MARK_PTR(value_start), MARK_LEN(value_start, p));
        }
    }
	break;
	case 9: 
#line 34 "tsv_parser.rl"
	{
        //printf("double \n");
    }
#line 24 "tsv_parser.rl"
	{
		if(ctx->on_record){
            ctx->on_record(ctx, MARK_PTR(key_start), ctx->key_len, MARK_PTR(value_start), MARK_LEN(value_start, p));
        }
    }
	break;
#line 335 "tsv_parser.c"
	}
	}

	_out: {}
	}

#line 89 "tsv_parser.rl"

    ctx->cs = cs;
    ctx->nread += p - (data + off);

    assert(p <= pe && "buffer overflow after parsing execute");
    assert(ctx->nread <= len && "nread longer than length");
    //assert(ctx->pos <= len && "body starts after buffer end");

    return(ctx->nread);
}



