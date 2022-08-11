#include "../lib/renderer/src/renderer.h"
#include <string>
#include <unity.h>
using namespace std;

/*
 * MVP - support a really nice bitmap / checkbox icon,
 *               and support checkboxes
 */

void test_renderer_table_formatting(void)
{
  // TODO - support single row table
  // TODO - what if the table is wider than the receipt paper?
  string a =
      "|name|fun hobby|\n"
      "|-|-|\n"
      "|nathan|embedded development|\n";
  vector<vector<string>> result = renderer(a);

  vector<vector<string>> expected{
      {"boldOn"},
      {"println", "| name   | fun hobby            |"},
      {"println", "| ------ | -------------------- |"},
      {"boldOff"},
      {"println", "| nathan | embedded development |"},
  };

  TEST_ASSERT_EQUAL(expected.size(), result.size());
  for (int x = 0; x < result.size(); x++)
  {
    for (int y = 0; y < result[x].size(); y++)
    {
      TEST_ASSERT_EQUAL_STRING(expected[x][y].c_str(), result[x][y].c_str());
    }
  }
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
  string a =
      "**bold**\n";
  vector<vector<string>> result = renderer(a);

  vector<vector<string>> expected{
      {"boldOn"},
      {"println", "bold"},
      {"boldOff"},
  };

  TEST_ASSERT_EQUAL(expected.size(), result.size());
  for (int x = 0; x < result.size(); x++)
  {
    for (int y = 0; y < result[x].size(); y++)
    {
      TEST_ASSERT_EQUAL_STRING(expected[x][y].c_str(), result[x][y].c_str());
    }
  }
}

void test_renderer_complex_text_formatting(void)
{
  string a =
      "# **bold**\n";
  vector<vector<string>> result = renderer(a);

  vector<vector<string>> expected{
      {"setSize", "L"},
      {"boldOn"},
      {"println", "bold"},
      {"boldOff"},
  };

  TEST_ASSERT_EQUAL(expected.size(), result.size());
  for (int x = 0; x < result.size(); x++)
  {
    for (int y = 0; y < result[x].size(); y++)
    {
      TEST_ASSERT_EQUAL_STRING(expected[x][y].c_str(), result[x][y].c_str());
    }
  }
}

void test_renderer_simple_heading(void)
{
  string a =
      "# h1\n"
      "## h2\n"
      "### h3\n";
  vector<vector<string>> result = renderer(a);

  vector<vector<string>> expected{
      {"setSize", "L"},
      {"println", "h1"},
      {"setSize", "M"},
      {"println", "h2"},
      {"setSize", "S"},
      {"println", "h3"},
  };

  TEST_ASSERT_EQUAL(expected.size(), result.size());
  for (int x = 0; x < result.size(); x++)
  {
    for (int y = 0; y < result[x].size(); y++)
    {
      TEST_ASSERT_EQUAL_STRING(expected[x][y].c_str(), result[x][y].c_str());
    }
  }
}

int main()
{
  UNITY_BEGIN();

  RUN_TEST(test_renderer_table_formatting);
  RUN_TEST(test_renderer_split_to_lines);
  RUN_TEST(test_renderer_simple_text_formatting);
  RUN_TEST(test_renderer_complex_text_formatting);
  RUN_TEST(test_renderer_simple_heading);

  UNITY_END();
}