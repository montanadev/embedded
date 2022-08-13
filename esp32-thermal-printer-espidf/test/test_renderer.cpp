#include "../lib/renderer/src/renderer.h"
#include <string>
#include <unity.h>
#include <iostream>

/*
 * MVP - support a really nice bitmap / checkbox icon,
 *               and support checkboxes
 */

void assert_rendered(string input, vector<vector<string> > expected)
{
  auto actual = renderer(input);

  TEST_ASSERT_EQUAL(expected.size(), actual.size());
  for (int x = 0; x < actual.size(); x++)
  {
    for (int y = 0; y < actual[x].size(); y++)
    {
      TEST_ASSERT_EQUAL_STRING(expected[x][y].c_str(), actual[x][y].c_str());
    }
  }
}

void test_split(void)
{
  string a = "|jkl|mno|";
  auto b = split(a, '\|');

  TEST_ASSERT_EQUAL(3, b.size());
  TEST_ASSERT_EQUAL_STRING("", b[0].c_str());
  TEST_ASSERT_EQUAL_STRING("jkl", b[1].c_str());
  TEST_ASSERT_EQUAL_STRING("mno", b[2].c_str());
}

void test_newlines(void)
{
  assert_rendered(
      "a\nb\nc\nd",
      {
          {"println", "a"},
          {"println", "b"},
          {"println", "c"},
          {"println", "d"},
      });
}

void test_renderer_table_single_column(void)
{
  assert_rendered(
      "|hello|\n"
      "|-------|\n"
      "| world!!!|\n",
      {
          {"boldOn"},
          {"println", "| hello    |"},
          {"println", "| -------- |"},
          {"boldOff"},
          {"println", "| world!!! |"},
      });
}

void test_renderer_table_formatting(void)
{
  // TODO - what if the table is wider than the receipt paper?
  assert_rendered(
      "|name|fun hobby|\n"
      "|-|-|\n"
      "|nathan|embedded development|\n",
      {
          {"boldOn"},
          {"println", "| name   | fun hobby            |"},
          {"println", "| ------ | -------------------- |"},
          {"boldOff"},
          {"println", "| nathan | embedded development |"},
      });

  assert_rendered(
      "| Option | Description |\n"
      "| ------ | ----------- |\n"
      "| data   | path to data |\n"
      "| engine | engine to be used |\n"
      "| ext    | extension to save |\n",
      {
          {"boldOn"},
          {"println", "| Option | Description       |"},
          {"println", "| ------ | ----------------- |"},
          {"boldOff"},
          {"println", "| data   | path to data      |"},
          {"println", "| engine | engine to be used |"},
          {"println", "| ext    | extension to save |"},
      });
}

void test_renderer_split_to_lines(void)
{
  string a =
      "# h1\n"
      "## h2\n"
      "### h3\n";
  vector<string> s = splitToLines(a);

  TEST_ASSERT_EQUAL(s.size(), 3);
  TEST_ASSERT_EQUAL_STRING(s[0].c_str(), "# h1");
  TEST_ASSERT_EQUAL_STRING(s[2].c_str(), "### h3");
}

void test_renderer_simple_text_formatting(void)
{
  assert_rendered(
      "** bold **\n#hi",
      {
          {"boldOn"},
          {"println", " bold "},
          {"boldOff"},
          {"setSize", "L"},
          {"println", "hi"},
          {"setSize", "S"},
      });
}

void test_renderer_horizontal_rule(void)
{
  assert_rendered(
      "--\n"
      "--- \n"
      " ----  \n",
      {
          {"println", "--"},
          {"setSize", "S"},
          {"boldOn"},
          {"println", "--------------------------------"},
          {"boldOff"},
          {"setSize", "S"},
          {"boldOn"},
          {"println", "--------------------------------"},
          {"boldOff"},
      });
}

void test_renderer_bold_header_formatting(void)
{
  assert_rendered(
      "# **bold**\n",
      {
          {"setSize", "L"},
          {"boldOn"},
          {"println", "bold"},
          {"boldOff"},
          {"setSize", "S"},
      });
}

void test_renderer_simple_heading(void)
{
  assert_rendered(
      " # h1\n"
      "  ## h2\n"
      "### h3\n"
      "  #### h4\n"
      "   ##### h5\n",
      {
          {"setSize", "L"},
          {"println", "h1"},
          {"setSize", "S"},
          {"setSize", "M"},
          {"println", "h2"},
          {"setSize", "S"},
          {"setSize", "S"},
          {"println", "h3"},
          {"setSize", "S"},
          {"println", "h4"},
          {"setSize", "S"},
          {"println", "h5"},
      });
}

int main()
{
  UNITY_BEGIN();

  RUN_TEST(test_split);
  RUN_TEST(test_newlines);
  RUN_TEST(test_renderer_horizontal_rule);
  RUN_TEST(test_renderer_table_single_column);
  RUN_TEST(test_renderer_table_formatting);
  RUN_TEST(test_renderer_split_to_lines);
  RUN_TEST(test_renderer_simple_text_formatting);
  RUN_TEST(test_renderer_bold_header_formatting);
  RUN_TEST(test_renderer_simple_heading);

  UNITY_END();
}