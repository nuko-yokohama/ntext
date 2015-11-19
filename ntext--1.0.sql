/* contrib/ntext/ntext--1.0.sql */

-- complain if script is sourced in psql, rather than via CREATE EXTENSION
\echo Use "CREATE EXTENSION ntext" to load this file. \quit


CREATE TYPE ntext;

CREATE FUNCTION ntext_in(cstring)
RETURNS ntext
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE FUNCTION ntext_out(ntext)
RETURNS cstring
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE TYPE ntext (
        INPUT = ntext_in,
        OUTPUT = ntext_out,
        STORAGE = plain
);

CREATE FUNCTION pg_ntext_normalize(ntext)
RETURNS ntext
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

-- 等しい
CREATE FUNCTION ntext_eq(ntext, ntext)
RETURNS bool
AS 'MODULE_PATHNAME'
LANGUAGE C IMMUTABLE STRICT;

CREATE OPERATOR = (
    leftarg = ntext,
    rightarg = ntext,
    procedure = ntext_eq,
    commutator = =,
    RESTRICT = eqsel
);

-- 等しくない
CREATE FUNCTION ntext_ne(ntext, ntext)
RETURNS bool
AS 'MODULE_PATHNAME'
LANGUAGE C IMMUTABLE STRICT;

CREATE OPERATOR <> (
    leftarg = ntext,
    rightarg = ntext,
    procedure = ntext_ne,
    RESTRICT = neqsel
);

-- より小さい
CREATE FUNCTION ntext_lt(ntext, ntext)
RETURNS bool
AS 'MODULE_PATHNAME'
LANGUAGE C IMMUTABLE STRICT;

CREATE OPERATOR < (
    leftarg = ntext,
    rightarg = ntext,
    procedure = ntext_lt,
    RESTRICT = scalarltsel
);


-- より大きい
CREATE FUNCTION ntext_gt(ntext, ntext)
RETURNS bool
AS 'MODULE_PATHNAME'
LANGUAGE C IMMUTABLE STRICT;

CREATE OPERATOR > (
    leftarg = ntext,
    rightarg = ntext,
    procedure = ntext_gt,
    RESTRICT = scalargtsel
);

-- 以下
CREATE FUNCTION ntext_le(ntext, ntext)
RETURNS bool
AS 'MODULE_PATHNAME'
LANGUAGE C IMMUTABLE STRICT;

CREATE OPERATOR <= (
    leftarg = ntext,
    rightarg = ntext,
    procedure = ntext_le,
    RESTRICT = scalarltsel
);


-- 以上
CREATE FUNCTION ntext_ge(ntext, ntext)
RETURNS bool
AS 'MODULE_PATHNAME'
LANGUAGE C IMMUTABLE STRICT;

CREATE OPERATOR >= (
    leftarg = ntext,
    rightarg = ntext,
    procedure = ntext_ge,
    RESTRICT = scalargtsel
);


-- B-treeサポートルーチン
CREATE FUNCTION ntext_cmp(ntext, ntext)
RETURNS integer
AS 'MODULE_PATHNAME'
LANGUAGE C IMMUTABLE STRICT;

-- ntext演算子クラス
CREATE OPERATOR CLASS ntext_ops
    DEFAULT FOR TYPE ntext USING btree AS
        OPERATOR        1       < ,
        OPERATOR        2       <= ,
        OPERATOR        3       = ,
        OPERATOR        4       >= ,
        OPERATOR        5       > ,
        FUNCTION        1       ntext_cmp(ntext, ntext);

-- 近似マッチ関数・演算子
CREATE FUNCTION pg_similar_rate(ntext, ntext)
RETURNS float4
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE FUNCTION vmatch(ntext, ntext)
RETURNS boolean
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE OPERATOR /= (
    leftarg = ntext,
    rightarg = ntext,
    procedure = vmatch,
    commutator = /=,
    RESTRICT = eqsel
);

