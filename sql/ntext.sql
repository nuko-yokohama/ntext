CREATE EXTENSION ntext;
\dx+ ntext
CREATE TABLE foo (id serial, data ntext);
\d foo
INSERT INTO foo (data) VALUES 
('味噌ラーメン')
,('ＰｏｓｔｇｒｅＳＱＬ')
,('ｴｳﾞｧﾝｹﾞﾘｦﾝ')
,('セリヌンティウス')
,('フィロストラトス')
;

TABLE foo;

SELECT * FROM foo WHERE data = '味噌らーめん';
SELECT * FROM foo WHERE data = 'postgresql';
SELECT * FROM foo WHERE data = 'エバンゲリオン';
SELECT * FROM foo WHERE data = 'センヌリティウス';
SELECT * FROM foo WHERE data = 'フィロストラス';
SELECT * FROM foo WHERE data /= 'センヌリティウス';
SELECT * FROM foo WHERE data /= 'フィロストラス';


DROP TABLE foo;
DROP EXTENSION ntext;


