
#include <driver.h>

#include <iostream>
#include <sstream>

#include "gtest/gtest.h"

class BubbleSortTest : public ::testing::Test
{
};

TEST_F(BubbleSortTest, ParseBubbleSortProgram)
{
    std::istringstream input(
        "program sort(input,output);\n"
        "var j,i,o:integer;\n"
        "var p :array [1..10] of integer;\n"
        "var b:integer;\n"
        "\n"
        "procedure czytajtab(a: array[1..10] of integer);\n"
        "var i: integer;\n"
        "begin\n"
        "  i:=1;\n"
        "  while i<11 do\n"
        "  begin\n"
        "    read(a[i]);\n"
        "    i:=i+1\n"
        "  end\n"
        "end;\n"
        "\n"
        "procedure bubblesort(a:array[1..10] of integer);\n"
        "var i,j:integer;\n"
        "var tmp: real;\n"
        "begin\n"
        "  i:=1;\n"
        "  while i<11 do \n"
        "  begin\n"
        "    j:=i+1;\n"
        "    while j<11 do begin\n"
        "      if a[i]>a[j] then\n"
        "       begin\n"
        "         tmp:=a[i];\n"
        " 	 a[i]:=a[j];\n"
        "	 a[j]:=tmp\n"
        "       end\n"
        "      else\n"
        "       begin\n"
        "       end;\n"
        "      j:=j+1\n"
        "     end;\n"
        "     i:=i+1\n"
        "  end\n"
        "end;\n"
        "\n"
        "procedure wypisztab(a: array[1..10] of integer);\n"
        "var i: integer;\n"
        "begin\n"
        "  i:=1;\n"
        "  while i<11 do\n"
        "  begin\n"
        "    write(a[i]);\n"
        "    i:=i+1\n"
        "    \n"
        "  end\n"
        "end;\n"
        "\n"
        "     \n"
        "begin\n"
        "  czytajtab(p);\n"
        "  bubblesort(p);\n"
        "  wypisztab(p)\n"
        "end.\n");

    const std::string expected =
        "jump.i #L0\n"
        "czytajtab:\n"
        "enter.i #20\n"
        "mov.i #1,BP-4\n"
        "L1:\n"
        "jl.i BP-4,#11,#L3\n"
        "mov.i #0,BP-8\n"
        "jump.i #L4\n"
        "L3:\n"
        "mov.i #1,BP-8\n"
        "L4:\n"
        "je.i BP-8,#0,#L2\n"
        "sub.i BP-4,#1,BP-12\n"
        "mul.i BP-12,#4,BP-12\n"
        "add.i BP+8,BP-12,BP-16\n"
        "read.i *BP-16\n"
        "add.i BP-4,#1,BP-20\n"
        "mov.i BP-20,BP-4\n"
        "jump.i #L1\n"
        "L2:\n"
        "leave\n"
        "return\n"
        "bubblesort:\n"
        "enter.i #100\n"
        "mov.i #1,BP-4\n"
        "L5:\n"
        "jl.i BP-4,#11,#L7\n"
        "mov.i #0,BP-20\n"
        "jump.i #L8\n"
        "L7:\n"
        "mov.i #1,BP-20\n"
        "L8:\n"
        "je.i BP-20,#0,#L6\n"
        "add.i BP-4,#1,BP-24\n"
        "mov.i BP-24,BP-8\n"
        "L9:\n"
        "jl.i BP-8,#11,#L11\n"
        "mov.i #0,BP-28\n"
        "jump.i #L12\n"
        "L11:\n"
        "mov.i #1,BP-28\n"
        "L12:\n"
        "je.i BP-28,#0,#L10\n"
        "sub.i BP-4,#1,BP-32\n"
        "mul.i BP-32,#4,BP-32\n"
        "add.i BP+8,BP-32,BP-36\n"
        "sub.i BP-8,#1,BP-40\n"
        "mul.i BP-40,#4,BP-40\n"
        "add.i BP+8,BP-40,BP-44\n"
        "jg.i *BP-36,*BP-44,#L13\n"
        "mov.i #0,BP-48\n"
        "jump.i #L14\n"
        "L13:\n"
        "mov.i #1,BP-48\n"
        "L14:\n"
        "je.i BP-48,#0,#L15\n"
        "sub.i BP-4,#1,BP-52\n"
        "mul.i BP-52,#4,BP-52\n"
        "add.i BP+8,BP-52,BP-56\n"
        "inttoreal.i *BP-56,BP-64\n"
        "mov.r BP-64,BP-16\n"
        "sub.i BP-4,#1,BP-68\n"
        "mul.i BP-68,#4,BP-68\n"
        "add.i BP+8,BP-68,BP-72\n"
        "sub.i BP-8,#1,BP-76\n"
        "mul.i BP-76,#4,BP-76\n"
        "add.i BP+8,BP-76,BP-80\n"
        "mov.i *BP-80,*BP-72\n"
        "sub.i BP-8,#1,BP-84\n"
        "mul.i BP-84,#4,BP-84\n"
        "add.i BP+8,BP-84,BP-88\n"
        "realtoint.r BP-16,BP-92\n"
        "mov.i BP-92,*BP-88\n"
        "jump.i #L16\n"
        "L15:\n"
        "L16:\n"
        "add.i BP-8,#1,BP-96\n"
        "mov.i BP-96,BP-8\n"
        "jump.i #L9\n"
        "L10:\n"
        "add.i BP-4,#1,BP-100\n"
        "mov.i BP-100,BP-4\n"
        "jump.i #L5\n"
        "L6:\n"
        "leave\n"
        "return\n"
        "wypisztab:\n"
        "enter.i #20\n"
        "mov.i #1,BP-4\n"
        "L17:\n"
        "jl.i BP-4,#11,#L19\n"
        "mov.i #0,BP-8\n"
        "jump.i #L20\n"
        "L19:\n"
        "mov.i #1,BP-8\n"
        "L20:\n"
        "je.i BP-8,#0,#L18\n"
        "sub.i BP-4,#1,BP-12\n"
        "mul.i BP-12,#4,BP-12\n"
        "add.i BP+8,BP-12,BP-16\n"
        "write.i *BP-16\n"
        "add.i BP-4,#1,BP-20\n"
        "mov.i BP-20,BP-4\n"
        "jump.i #L17\n"
        "L18:\n"
        "leave\n"
        "return\n"
        "L0:\n"
        "push.i #12\n"
        "call.i #czytajtab\n"
        "incsp.i #4\n"
        "push.i #12\n"
        "call.i #bubblesort\n"
        "incsp.i #4\n"
        "push.i #12\n"
        "call.i #wypisztab\n"
        "incsp.i #4\n"
        "exit\n";

    std::ostringstream output;
    Driver driver(output, input);

    ASSERT_EQ(driver.parse(), 0);
    ASSERT_EQ(output.str(), expected);
}
