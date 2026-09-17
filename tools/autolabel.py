#!/usr/bin/env python3
"""Generate provisional function names from text and file-name evidence.

    python3 tools/autolabel.py build/ghidra/evidence.tsv > tools/ghidra/war2_auto_labels.txt

The evidence comes from tools/ghidra/CollectEvidence.java. Every generated name
starts with `auto_`, and SetupWar2.java applies one only to a function that
still has a default FUN_ name (or an older auto_ name), so hand-made labels in
war2_labels.txt always win.

Priority per function:
  1. data files it names  (FILE.DAT lookups, or literal paths)  auto_file_*
  2. STRING.DAT groups it shows                                  auto_ui_*
  3. sound / advisor-voice files                                 auto_sound_*
  4. ERROR.DAT messages                                          auto_err_*
  5. other literal strings                                       auto_str_*
  6. only a tutorial-page lookup                                 auto_tutorial_hook*
"""
import collections
import os
import re
import sys

sys.path.insert(0, os.path.dirname(__file__))
from string_dat import load  # noqa: E402

ROOT = os.path.join(os.path.dirname(__file__), '..', 'original', 'DATA')
LOAD_SEG = 0x1000

# One short topic per STRING.DAT group (see docs/formats/string.md).
UI_GROUPS = [
    'random_world', 'new_scenario', 'random_map_terrain', 'allies_option',
    'game_option_names', 'neutral_strength', 'difficulty_rating', 'options_menu',
    'game_options', 'difficulty_affecting', 'difficulty_not_affecting',
    'player_eliminated', 'no_players_left', 'no_humans_left', 'no_humans_playing',
    'player_triumphs', 'inspect_kingdom', 'surrender_offer', 'all_cities_seen',
    'temple', 'no_quest', 'quest_slay_hero', 'quest_retrieve_item',
    'quest_slay_unit', 'quest_slaughter_armies', 'quest_occupy_city',
    'quest_conquer_city', 'quest_pillage_gold', 'quest_completed',
    'quest_reward_site', 'quest_reward_item', 'quest_reward_allies',
    'quest_hero_dead', 'quest_city_razed', 'quest_invalid', 'quest_invalid',
    'quest_invalid', 'quest_invalid', 'quest_invalid', 'quest_invalid',
    'quest_invalid', 'quest_invalid', 'quest_invalid', 'observe_hidden_map',
    'quit_game', 'new_game', 'save_game', 'load_game', 'save_map', 'load_map',
    'ruin_uninhabited', 'ruin_encounter', 'found_item', 'found_gold',
    'allies_join', 'already_blessed', 'blessed', 'site_info', 'found_sage',
    'sage_gem', 'sage_map', 'ruin_status', 'sage_locations', 'sage_greeting',
    'what', 'city_victory', 'victory_title', 'victory_where',
    'pillage_sack_raze', 'city_pillaged', 'pillage_gold', 'production_lost',
    'production_remaining', 'reports_menu', 'report_descriptions',
    'report_armies', 'report_cities', 'report_gold', 'report_production',
    'report_ranking', 'triumphs', 'triumphs', 'triumphs', 'triumphs', 'triumphs',
    'triumphs', 'triumphs', 'triumphs', 'triumphs', 'triumphs', 'triumphs',
    'history_menu', 'history_lines', 'history_empty', 'history_events',
    'hero_offer', 'hero_allies', 'hero_emerges', 'hero_gender',
    'hero_levels_male', 'hero_levels_female', 'hero_promote_cavalier',
    'hero_promote_champion', 'hero_promote_paladin', 'hero_levels',
    'army_ability_tags', 'diplomacy_ratings', 'diplomacy_report',
    'diplomacy_action', 'diplomacy_rating', 'diplomacy_state',
    'diplomacy_events', 'build_production', 'site_type', 'site_explored',
    'site_status', 'city_info', 'vector_current', 'vector_orders',
    'rename_city', 'raze_city', 'buy_production', 'select_city',
    'advisor', 'advisor_greeting', 'advisor_battle_intro',
    'advisor_battle_odds', 'fighting_order', 'terrain_names',
    'terrain_descriptions', 'tower', 'army_info_labels', 'army_info',
    'loading_saved_game', 'loading_saved_map', 'loading_scenario',
    'creating_random_map', 'creating_strategic_map', 'setting_up_game',
    'version', 'war_undeclared', 'garrison_fled', 'hero_won_battle',
    'won_city', 'won_battle', 'you_are_victorious', 'you_have_lost',
    'loot_gold', 'being_attacked', 'neutrals_attacked', 'attack_report_won',
    'neutrals_victorious', 'attack_report_lost', 'neutrals_lost',
    'production_timer', 'vector_help', 'change_destination_help', 'medal',
    'medal_reason', 'medal_unit', 'medal_awarded', 'medal_names',
    'medal_effect', 'army_bonus_text', 'army_bonus', 'resign', 'items',
    'items_list', 'armies_sunk',
]

DATA_EXT = re.compile(r'\.(dat|map|rd|scn|sgn|spc|cty|itm|pck|gfx|hst|xmi|fnt|fin|pal|hlp|cur)$', re.I)
SOUND_EXT = re.compile(r'\.(8sn|txt)$', re.I)
# FILE.DAT group 25 lists the TUTORIA\\*.GFX pages; many ordinary functions
# look it up to trigger a tutorial page, so it says little about them.
TUTORIAL_GROUP = 25
PATH = re.compile(r'^[\w%\\.]*\.[A-Za-z0-9]{2,3}$')


def slug(text, words=4):
    text = re.sub(r'\\+[nr]', ' ', text)   # escaped newlines from the evidence file
    parts = re.findall(r'[a-z0-9]+', text.lower().replace('%d', '').replace('%s', ''))
    return '_'.join(parts[:words])


def file_slug(path):
    parts = [p for p in re.split(r'\\+', path) if p]
    base = parts[-1] if parts else path
    if re.search(r'\\$', path):      # a bare directory such as SOUND\\
        base += '_dir'
    return slug(base.replace('%d', '').replace('%s', 'x'), 3)


def ghidra_to_flat(addr):
    seg, off = addr.split(':')
    return '%04x:%s' % (int(seg, 16) - LOAD_SEG, off)


def main(path):
    strings = load(os.path.join(ROOT, 'STRING.DAT'))
    errors = load(os.path.join(ROOT, 'ERROR.DAT'))
    files = load(os.path.join(ROOT, 'FILE.DAT'))
    assert len(UI_GROUPS) == len(strings), (len(UI_GROUPS), len(strings))

    ev = collections.defaultdict(lambda: dict(name=None, ui=collections.Counter(),
                                              data=[], sound=[], err=[], str=[],
                                              tutorial=False))
    for line in open(path, encoding='latin1'):
        f = line.rstrip('\n').split('\t')
        e = ev[f[0]]
        e['name'] = f[1]
        if f[2] == 'text':
            table, group, index = int(f[3]), int(f[4]), f[5]
            if table == 0 and group < len(strings):
                e['ui'][group] += 1
            elif table == 2 and group < len(errors):
                e['err'].append(errors[group][0])
            elif table == 1 and group == TUTORIAL_GROUP:
                e['tutorial'] = True   # a tutorial-page hook, not a loader
            elif table == 1 and group < len(files):
                g = files[group]
                if index != '?' and 0 <= int(index) < len(g):
                    name = g[int(index)]
                elif len(g) > 1:
                    # variable index into a multi-file group: name the group
                    parts = [p for p in re.split(r'\\+', g[0]) if p]
                    ext = g[0].rsplit('.', 1)[-1] if '.' in g[0] else 'file'
                    name = (parts[0] if len(parts) > 1 else 'files') + '_any.' + ext
                else:
                    name = g[0]
                (e['sound'] if SOUND_EXT.search(name) or name.upper().startswith('SOUND')
                 else e['data']).append(name)
        elif f[2] == 'str':
            s = f[4]
            if PATH.match(s) and DATA_EXT.search(s):
                e['data'].append(s)
            elif PATH.match(s) and SOUND_EXT.search(s):
                e['sound'].append(s)
            elif len(re.findall(r'[A-Za-z]{3,}', s)) >= 2:
                e['str'].append(s)

    used = collections.Counter()
    rows = []
    for addr in sorted(ev, key=lambda a: (int(a[:4], 16), int(a[5:], 16))):
        e = ev[addr]
        if not (e['name'].startswith('FUN_') or e['name'].startswith('auto_')):
            continue
        if e['data']:
            base, why = 'file_' + file_slug(e['data'][0]), ', '.join(dict.fromkeys(e['data']))
        elif e['ui']:
            top = sorted(e['ui'].items(), key=lambda kv: (-kv[1], kv[0]))
            base = 'ui_' + UI_GROUPS[top[0][0]]
            why = 'STRING.DAT ' + ', '.join('%d×%d' % (g, n) for g, n in top)
        elif e['sound']:
            base, why = 'sound_' + file_slug(e['sound'][0]), ', '.join(dict.fromkeys(e['sound']))
        elif e['err']:
            base, why = 'err_' + slug(e['err'][0]), 'ERROR.DAT: ' + e['err'][0]
        elif e['str']:
            base, why = 'str_' + slug(e['str'][0]), repr(e['str'][0])
        elif e['tutorial']:
            base, why = 'tutorial_hook', 'FILE.DAT group 25 (tutorial pages)'
        else:
            continue
        used[base] += 1
        name = 'auto_' + base + ('_%d' % used[base] if used[base] > 1 else '')
        rows.append((ghidra_to_flat(addr), name, why))

    print('# GENERATED by tools/autolabel.py from CollectEvidence.java output.')
    print('# Do not edit: put confirmed names in war2_labels.txt instead.')
    print('# Flat-EXE addresses; applied only to functions still named FUN_*/auto_*.')
    for addr, name, why in rows:
        print('%s  func  %-40s # %s' % (addr, name, why[:100].replace('#', '')))
    print('%d functions named' % len(rows), file=sys.stderr)


if __name__ == '__main__':
    main(sys.argv[1] if len(sys.argv) > 1 else 'build/ghidra/evidence.tsv')
