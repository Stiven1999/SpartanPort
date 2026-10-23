"""Launcher setting for dynamic vertex input (SPARTAN_VERTEX_INPUT_DYNAMIC): Automatic leaves the game's
own default (off on Intel's Windows driver, spartanport_vertex_input_default.h); On and Off force it."""
import os
import sys
import unittest
from unittest import mock

from paths import ROOT
sys.path.insert(0, str(ROOT / 'launcher'))
import spartanport_launcher_win as launcher  # noqa: E402


def added(env):
    return {key for key in env if key not in os.environ}


class VertexInputEnvironmentTest(unittest.TestCase):
    def test_auto_leaves_the_game_default_and_keeps_an_inherited_value(self):
        self.assertEqual(launcher.APP_DEFAULTS['vertex_input'], '')
        env = launcher.game_environment(dict(launcher.APP_DEFAULTS))
        self.assertNotIn('SPARTAN_VERTEX_INPUT_DYNAMIC', added(env))
        with mock.patch.dict(os.environ, {'SPARTAN_VERTEX_INPUT_DYNAMIC': '1'}):
            env = launcher.game_environment(dict(launcher.APP_DEFAULTS))
            self.assertEqual(env['SPARTAN_VERTEX_INPUT_DYNAMIC'], '1')

    def test_on_and_off_set_the_variable(self):
        for value in ('1', '0'):
            with self.subTest(value=value):
                env = launcher.game_environment({**launcher.APP_DEFAULTS, 'vertex_input': value})
                self.assertEqual(env['SPARTAN_VERTEX_INPUT_DYNAMIC'], value)

    def test_choices_and_legacy_settings(self):
        self.assertEqual([value for value, _text in launcher.VERTEX_INPUT], ['', '1', '0'])
        legacy = dict(launcher.APP_DEFAULTS)
        legacy.pop('vertex_input')
        self.assertNotIn('SPARTAN_VERTEX_INPUT_DYNAMIC', added(launcher.game_environment(legacy)))


class VertexInputTranslationTest(unittest.TestCase):
    def test_every_launcher_language_translates_the_texts(self):
        import spartanport_lang
        texts = ['Dynamic vertex input', launcher.VERTEX_INPUT[2][1][0], launcher.VERTEX_INPUT_HINT]
        for language, _name in launcher.UI_LANGUAGES:
            if language in ('', 'en', 'ru'):
                continue
            table = spartanport_lang.table(language)
            for text in texts:
                with self.subTest(language=language, text=text[:30]):
                    self.assertNotEqual(table.get(text, text), text)

    def test_russian_is_next_to_the_english_text(self):
        for _value, text in launcher.VERTEX_INPUT:
            self.assertEqual(len(text), 2)
        self.assertTrue(launcher.VERTEX_INPUT_HINT_RU)


if __name__ == '__main__':
    unittest.main()
