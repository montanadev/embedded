#include "../lib/renderer/src/renderer.h"
#include <string>
#include <unity.h>

/*
 * MVP - support a really nice bitmap / checkbox icon,
 *               and support checkboxes
 */

void test_split(void) {
  string a = "|jkl|mno|";
  auto b = split(a, '\|');
  
  TEST_ASSERT_EQUAL(3, b.size());
  TEST_ASSERT_EQUAL_STRING("", b[0].c_str());
  TEST_ASSERT_EQUAL_STRING("jkl", b[1].c_str());
  TEST_ASSERT_EQUAL_STRING("mno", b[2].c_str());
}

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

  a =
      "| Option | Description |\n"
      "| ------ | ----------- |\n"
      "| data   | path to data |\n"
      "| engine | engine to be used |\n"
      "| ext    | extension to save |\n";
  result = renderer(a);

  expected = {
      {"boldOn"},
      {"println", "| Option | Description       |"},
      {"println", "| ------ | ----------------- |"},
      {"boldOff"},
      {"println", "| data   | path to data      |"},
      {"println", "| engine | engine to be used |"},
      {"println", "| ext    | extension to save |"},
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
      {"setSize", "S"},
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
      "### h3\n"
      "#### h4\n"
      "##### h5\n";
  vector<vector<string>> result = renderer(a);

  vector<vector<string>> expected{
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

  RUN_TEST(test_split);
  RUN_TEST(test_renderer_table_formatting);
  RUN_TEST(test_renderer_split_to_lines);
  RUN_TEST(test_renderer_simple_text_formatting);
  RUN_TEST(test_renderer_complex_text_formatting);
  RUN_TEST(test_renderer_simple_heading);

  UNITY_END();
}