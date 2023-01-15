program example(input, output);
var x: integer;
var y: array [0..10] of integer;

function f(x: integer): integer;
begin
  f := 2*x
end;

procedure w(x: integer);
begin
  if x>0 then write(x) else write(0)
end;

procedure p(x: integer; y: array [0..10] of integer);
begin
  while x > 0 do
  begin
    x := x - 1;
    w(f(y[x]))
  end
end;

begin

read(x, y[0], y[1], y[2]);
p(x, y)
end.
