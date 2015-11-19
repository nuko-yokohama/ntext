#include <stdio.h>
#include <string.h>

#include "ntext.h"
#include "similar.h"

PG_MODULE_MAGIC;

PG_FUNCTION_INFO_V1(ntext_in);
Datum ntext_in(PG_FUNCTION_ARGS);

Datum ntext_in(PG_FUNCTION_ARGS) {
        char       *text = PG_GETARG_CSTRING(0);

        /* 現バージョンでは内部表現はTEXT型と同じにしている */
        /* TODO: 正規化した表現も型の内部表現として一緒にもたせるべきかもしれない */
        PG_RETURN_TEXT_P(cstring_to_text(text));

}

PG_FUNCTION_INFO_V1(ntext_out);
Datum ntext_out(PG_FUNCTION_ARGS);

Datum ntext_out(PG_FUNCTION_ARGS) {
        Datum           txt = PG_GETARG_DATUM(0);

        /* 現バージョンではTEXT型の出力関数と同じ */
        /* TODO: 内部表現として正規化テキストを持たせた場合も、 */
        /* 出力形式はオリジナルの表現を返却すべきだろう */
        PG_RETURN_CSTRING(TextDatumGetCString(txt));
}

PG_FUNCTION_INFO_V1(pg_ntext_normalize);
Datum pg_ntext_normalize(PG_FUNCTION_ARGS);

/**
 * ntext正規化関数
 */
Datum pg_ntext_normalize(PG_FUNCTION_ARGS) {
    text* text_a = PG_GETARG_TEXT_PP(0);
    char* a = text_to_cstring(text_a);
    wchar_t* ws_a; /* 正規化前のワイド文字列 */
    wchar_t* ws_n_a; /* 正規化されたワイド文字列 */
    int a_len;
    int a_ws_len;
    char* result;

    setlocale( LC_CTYPE, "ja_JP.UTF-8" );

    a_len = strlen(a);
    ws_a = (wchar_t*) palloc(sizeof(wchar_t) * (a_len + 1));
    ws_n_a = (wchar_t*) palloc(sizeof(wchar_t) * (a_len + 1));
    memset(ws_a, '\0', sizeof(wchar_t) * (a_len + 1));
    memset(ws_n_a, '\0', sizeof(wchar_t) * (a_len + 1));

// elog(NOTICE, "a=%s", a);
    mbstowcs( ws_a, a, a_len);
    a_ws_len = wcslen(ws_a);
// elog(NOTICE, "ws_a=%ls", ws_a);
    /* 日本語正規化処理 */
    ntext_normalize(ws_a, ws_n_a, a_ws_len);
    result = palloc(sizeof(wchar_t) * (a_len + 1));
    memset(result, '\0', sizeof(wchar_t) * (a_len + 1));
// elog(NOTICE, "ws_n_a=%ls", ws_n_a);
    wcstombs( result, ws_n_a, (sizeof(wchar_t) * wcslen(ws_n_a)));
    
    pfree(ws_a);
    pfree(ws_n_a);
// elog(NOTICE, "result=%s", result);
    PG_RETURN_TEXT_P(cstring_to_text(result));
}

/**
 * 比較演算子
 * 比較用の内部関数を作成しておくと便利
 */
static int
ntext_cmp_internal(char* a, char* b)
{
    int a_len;
    int b_len;
    int a_ws_len;
    int b_ws_len;
    wchar_t* ws_a; /* 正規化前のワイド文字列 */
    wchar_t* ws_b; /* 正規化前のワイド文字列 */
    wchar_t* ws_n_a; /* 正規化されたワイド文字列 */
    wchar_t* ws_n_b; /* 正規化されたワイド文字列 */
    int result;

    /* TODO: pg_init()で呼ぶべきか？ */
    setlocale( LC_CTYPE, "ja_JP.UTF-8" );
    /* TODO: エラー処理をきちんと組み込むべき */
    a_len = strlen(a);
    b_len = strlen(b);
    ws_a = (wchar_t*) palloc(sizeof(wchar_t) * (a_len + 1));
    ws_b = (wchar_t*) palloc(sizeof(wchar_t) * (b_len + 1));
    ws_n_a = (wchar_t*) palloc(sizeof(wchar_t) * (a_len + 1));
    ws_n_b = (wchar_t*) palloc(sizeof(wchar_t) * (b_len + 1));
    /* 念のためクリア */
    memset(ws_a, '\0', sizeof(wchar_t) * (a_len + 1));
    memset(ws_b, '\0', sizeof(wchar_t) * (b_len + 1));
    memset(ws_n_a, '\0', sizeof(wchar_t) * (a_len + 1));
    memset(ws_n_b, '\0', sizeof(wchar_t) * (b_len + 1));
    /* マルチバイトからワイドキャラへ変換 */
    mbstowcs( ws_a, a, a_len);
    a_ws_len = wcslen(ws_a);
    mbstowcs( ws_b, b, b_len);
    b_ws_len = wcslen(ws_b);
    /* 日本語正規化処理 */
    ntext_normalize(ws_a, ws_n_a, a_ws_len);
//  elog(NOTICE, "ws_n_a=%ls, a_ws_len=%d", ws_n_a, a_ws_len);
    ntext_normalize(ws_b, ws_n_b, b_ws_len);
//  elog(NOTICE, "ws_n_b=%ls, b_ws_len=%d", ws_n_b, b_ws_len);

    /* 正規化した日本語を比較 */
    result = wcscmp(ws_n_a, ws_n_b);
    pfree(ws_a);
    pfree(ws_b);
    pfree(ws_n_a);
    pfree(ws_n_b);
    return result;
}

// eq 演算子関数
PG_FUNCTION_INFO_V1(ntext_eq);
Datum ntext_eq(PG_FUNCTION_ARGS);

Datum
ntext_eq(PG_FUNCTION_ARGS)
{
    char* text_a = PG_GETARG_TEXT_PP(0);
    char* text_b = PG_GETARG_TEXT_PP(1);
    char* a = text_to_cstring(text_a);
    char* b = text_to_cstring(text_b);

    PG_RETURN_BOOL(ntext_cmp_internal( a, b) == 0);
}

// ne 演算子関数
PG_FUNCTION_INFO_V1(ntext_ne);
Datum ntext_ne(PG_FUNCTION_ARGS);

Datum
ntext_ne(PG_FUNCTION_ARGS)
{
    char* text_a = PG_GETARG_TEXT_PP(0);
    char* text_b = PG_GETARG_TEXT_PP(1);
    char* a = text_to_cstring(text_a);
    char* b = text_to_cstring(text_b);

    PG_RETURN_BOOL(ntext_cmp_internal( a, b) != 0);
}

// lt 演算子関数
PG_FUNCTION_INFO_V1(ntext_lt);
Datum ntext_lt(PG_FUNCTION_ARGS);

Datum
ntext_lt(PG_FUNCTION_ARGS)
{
    char* text_a = PG_GETARG_TEXT_PP(0);
    char* text_b = PG_GETARG_TEXT_PP(1);
    char* a = text_to_cstring(text_a);
    char* b = text_to_cstring(text_b);

    PG_RETURN_BOOL(ntext_cmp_internal( a, b) < 0);
}

// gt 演算子関数
PG_FUNCTION_INFO_V1(ntext_gt);
Datum ntext_gt(PG_FUNCTION_ARGS);

Datum
ntext_gt(PG_FUNCTION_ARGS)
{
    char* text_a = PG_GETARG_TEXT_PP(0);
    char* text_b = PG_GETARG_TEXT_PP(1);
    char* a = text_to_cstring(text_a);
    char* b = text_to_cstring(text_b);

    PG_RETURN_BOOL(ntext_cmp_internal( a, b) > 0);
}

// le 演算子関数
PG_FUNCTION_INFO_V1(ntext_le);
Datum ntext_le(PG_FUNCTION_ARGS);

Datum
ntext_le(PG_FUNCTION_ARGS)
{
    char* text_a = PG_GETARG_TEXT_PP(0);
    char* text_b = PG_GETARG_TEXT_PP(1);
    char* a = text_to_cstring(text_a);
    char* b = text_to_cstring(text_b);

    PG_RETURN_BOOL(ntext_cmp_internal( a, b) <= 0);
}

// ge 演算子関数
PG_FUNCTION_INFO_V1(ntext_ge);
Datum ntext_ge(PG_FUNCTION_ARGS);

Datum
ntext_ge(PG_FUNCTION_ARGS)
{
    char* text_a = PG_GETARG_TEXT_PP(0);
    char* text_b = PG_GETARG_TEXT_PP(1);
    char* a = text_to_cstring(text_a);
    char* b = text_to_cstring(text_b);

    PG_RETURN_BOOL(ntext_cmp_internal( a, b) >= 0);
}

// 
// btreeインデクスサポート
//
// cmp演算子関数
PG_FUNCTION_INFO_V1(ntext_cmp);
Datum ntext_cmp(PG_FUNCTION_ARGS);

Datum
ntext_cmp(PG_FUNCTION_ARGS)
{
    char* text_a = PG_GETARG_CSTRING(0);
    char* text_b = PG_GETARG_CSTRING(1);
    char* a = text_to_cstring(text_a);
    char* b = text_to_cstring(text_b);

    PG_RETURN_INT32(ntext_cmp_internal( a, b));
}

/**
 * vmatch
 * ワイドキャラ近似検索演算子
 **/
PG_FUNCTION_INFO_V1(vmatch);
Datum vmatch(PG_FUNCTION_ARGS);

#define VMATCH_LIMIT 0.60

Datum vmatch(PG_FUNCTION_ARGS) {
    text* txt1 = PG_GETARG_TEXT_P(0);
    text* txt2 = PG_GETARG_TEXT_P(1);
    char* str1 = text_to_cstring(txt1);
    char* str2 = text_to_cstring(txt2);

    wchar_t* w_str1; /* 正規化前 */
    wchar_t* w_str2; /* 正規化前 */
    wchar_t* w_n_str1; /* 正規化後 */
    wchar_t* w_n_str2; /* 正規化後 */
    int w_str1_len, w_str2_len, w_n_str1_len, w_n_str2_len;

    float4 rate = 0.0;

    setlocale( LC_CTYPE, "ja_JP.UTF-8" );

// elog(NOTICE, "str_1 = %s", str1);
// elog(NOTICE, "str_2 = %s", str2);

    w_str1 = (wchar_t*) palloc(sizeof(wchar_t) * strlen(str1 + 1));
    w_str2 = (wchar_t*) palloc(sizeof(wchar_t) * strlen(str2 + 1));
    w_n_str1 = (wchar_t*) palloc(sizeof(wchar_t) * strlen(str1 + 1));
    w_n_str2 = (wchar_t*) palloc(sizeof(wchar_t) * strlen(str1 + 1));
    /* 念のためクリア */
    memset(w_str1, '\0', sizeof(wchar_t) * (strlen(str1) + 1));
    memset(w_str2, '\0', sizeof(wchar_t) * (strlen(str2) + 1));
    memset(w_n_str1, '\0', sizeof(wchar_t) * (strlen(str1) + 1));
    memset(w_n_str2, '\0', sizeof(wchar_t) * (strlen(str2) + 1));

    /* マルチバイトからワイドキャラへ変換 */
    mbstowcs( w_str1, str1, strlen(str1));
    w_str1_len = wcslen(w_str1);
    mbstowcs( w_str2, str2, strlen(str2));
    w_str2_len = wcslen(w_str2);
    /* 日本語正規化処理 */
// elog(NOTICE, "w_str_1 = %ls", w_str1);
// elog(NOTICE, "w_str_2 = %ls", w_str2);
    ntext_normalize(w_str1, w_n_str1, w_str1_len);
    w_n_str1_len = wcslen(w_n_str1);
    ntext_normalize(w_str2, w_n_str2, w_str2_len);
    w_n_str2_len = wcslen(w_n_str2);
// elog(NOTICE, "w_n_str_1 = %ls", w_n_str1);
// elog(NOTICE, "w_n_str_2 = %ls", w_n_str2);

// elog(NOTICE, "vmatch: rate = %f", rate);
    rate = similar_rate(w_n_str1, w_n_str2);
// elog(NOTICE, "vmatch: rate = %f", rate);

    if (rate >= VMATCH_LIMIT) {
        PG_RETURN_BOOL(true);
    } else {
        PG_RETURN_BOOL(false);
    }
}

PG_FUNCTION_INFO_V1(pg_similar_rate);
Datum pg_similar_rate(PG_FUNCTION_ARGS);

Datum pg_similar_rate(PG_FUNCTION_ARGS) {
    text* txt1 = PG_GETARG_TEXT_P(0);
    text* txt2 = PG_GETARG_TEXT_P(1);
    char* str1 = text_to_cstring(txt1);
    char* str2 = text_to_cstring(txt2);

    wchar_t* w_str1; /* 正規化前 */
    wchar_t* w_str2; /* 正規化前 */
    wchar_t* w_n_str1; /* 正規化後 */
    wchar_t* w_n_str2; /* 正規化後 */
    int w_str1_len, w_str2_len, w_n_str1_len, w_n_str2_len;

    float4 rate = 0.0;
    float4* result;

    setlocale( LC_CTYPE, "ja_JP.UTF-8" );

// elog(NOTICE, "str_1 = %s", str1);
// elog(NOTICE, "str_2 = %s", str2);

    w_str1 = (wchar_t*) palloc(sizeof(wchar_t) * strlen(str1 + 1));
    w_str2 = (wchar_t*) palloc(sizeof(wchar_t) * strlen(str2 + 1));
    w_n_str1 = (wchar_t*) palloc(sizeof(wchar_t) * strlen(str1 + 1));
    w_n_str2 = (wchar_t*) palloc(sizeof(wchar_t) * strlen(str1 + 1));
    /* 念のためクリア */
    memset(w_str1, '\0', sizeof(wchar_t) * (strlen(str1) + 1));
    memset(w_str2, '\0', sizeof(wchar_t) * (strlen(str2) + 1));
    memset(w_n_str1, '\0', sizeof(wchar_t) * (strlen(str1) + 1));
    memset(w_n_str2, '\0', sizeof(wchar_t) * (strlen(str2) + 1));

    /* マルチバイトからワイドキャラへ変換 */
    mbstowcs( w_str1, str1, strlen(str1));
    w_str1_len = wcslen(w_str1);
    mbstowcs( w_str2, str2, strlen(str2));
    w_str2_len = wcslen(w_str2);
    /* 日本語正規化処理 */
// elog(NOTICE, "w_str_1 = %ls", w_str1);
// elog(NOTICE, "w_str_2 = %ls", w_str2);
    ntext_normalize(w_str1, w_n_str1, w_str1_len);
    w_n_str1_len = wcslen(w_n_str1);
    ntext_normalize(w_str2, w_n_str2, w_str2_len);
    w_n_str2_len = wcslen(w_n_str2);
// elog(NOTICE, "w_n_str_1 = %ls", w_n_str1);
// elog(NOTICE, "w_n_str_2 = %ls", w_n_str2);

// elog(NOTICE, "vmatch: rate = %f", rate);
    rate = similar_rate(w_n_str1, w_n_str2);
// elog(NOTICE, "vmatch: rate = %f", rate);

    result = (float4*) palloc(sizeof(float));
    *result = rate;

    PG_RETURN_FLOAT4( *result);
}

