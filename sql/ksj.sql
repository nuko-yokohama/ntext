CREATE EXTENSION ksj;

CREATE TABLE ksj_sample (data ksj);
INSERT INTO ksj_sample VALUES ('五百拾六'),('零'),('弐万百壱');
SELECT * FROM ksj_sample;

SELECT ksj_add('百','拾');
SELECT ksj_sub('百','拾');
SELECT ksj_mul('百','拾');
SELECT ksj_div('百','拾');
SELECT ksj_mod('百','拾');
SELECT '百'::ksj + '拾'::ksj;
SELECT '百'::ksj - '拾'::ksj;
SELECT '百'::ksj * '拾'::ksj;
SELECT '百'::ksj / '拾'::ksj;
SELECT '百'::ksj % '拾'::ksj;
SELECT data, data + '拾' FROM ksj_sample;
SELECT data, data * '拾' FROM ksj_sample;
SELECT data, data / '拾' FROM ksj_sample;
SELECT data, data % '拾' FROM ksj_sample;

SELECT sum(data) FROM ksj_sample;
SELECT max(data) FROM ksj_sample;
SELECT min(data) FROM ksj_sample;

SELECT * FROM ksj_sample WHERE data = '五百拾六';
SELECT * FROM ksj_sample WHERE data <> '五百拾六';
SELECT * FROM ksj_sample WHERE data < '五百拾六';
SELECT * FROM ksj_sample WHERE data > '五百拾六';
SELECT * FROM ksj_sample WHERE data <= '五百拾六';
SELECT * FROM ksj_sample WHERE data >= '五百拾六';

SELECT * FROM ksj_sample ORDER BY data;
SELECT * FROM ksj_sample ORDER BY data::text;
SELECT * FROM ksj_sample ORDER BY data DESC;

CREATE INDEX ksj_idx ON ksj_sample (data);
SET enable_seqscan = off;
EXPLAIN SELECT data FROM ksj_sample WHERE data = '五百拾六';

TRUNCATE TABLE ksj_sample;
INSERT INTO ksj_sample VALUES (516::ksj),(0::ksj),(20101::ksj);
SELECT data FROM ksj_sample;
SELECT data::integer FROM ksj_sample;

DROP TABLE ksj_sample;
DROP EXTENSION ksj;
