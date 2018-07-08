import sqlite3


# Usage: place this script near eve.db downloaded from:
#    https://www.fuzzwork.co.uk/dump/latest/eve.db.bz2
# Run this script, it will generate all needed .sql files
#    to be imported by qmlevemon app.


def nv(v, escape_quote: bool = False) -> str:
    if v is None:
        return 'NULL'
    tv = str(v)
    if escape_quote:
        tv = tv.replace('\'', '\'\'');
    return tv


def write_table_dump(db: sqlite3.Connection, tbl_name: str, columns: list):
    filename = '{}.sql'.format(tbl_name)
    cur = db.cursor()
    cur.execute('PRAGMA table_info({})'.format(tbl_name))
    print('{} structure:'.format(tbl_name))
    # idx, column_name, data_type, not_null, default_value, is_pk
    column_data = []
    for row in cur.fetchall():
        if row[1] not in columns:
            continue
        cdata = {
            'column_name': row[1],
            'data_type': row[2],
            'not_null': row[3],
            'default_value': row[4],
            'is_pk': row[5]
        }
        column_data.append(cdata)

    create_stmt = 'CREATE TABLE \"{}\" ( \n'.format(tbl_name)
    primary_keys = []
    i = 0
    for cdata in column_data:
        if i > 0:
            create_stmt += ', \n'
        create_stmt += '  \"' + cdata['column_name'] + '\" '
        create_stmt += cdata['data_type']
        if cdata['not_null'] > 0:
            create_stmt += ' NOT NULL'
        if cdata['default_value'] is not None:
            create_stmt += ' DEFAULT \"' + cdata['default_value'] + '\"'
        if cdata['is_pk'] > 0:
            primary_keys.append(cdata['column_name'])
        i += 1
    i = 0
    for pk in primary_keys:
        if i == 0:
            create_stmt += '\n'
            create_stmt += '  PRIMARY KEY ('
        else:
            create_stmt += ', '
        create_stmt += '\"' + pk + '\"'
        i += 1
    if i > 0:
        create_stmt += ') \n'
    create_stmt += ');'

    print(create_stmt)

    num_rows = 0
    with open(filename, 'wt', encoding='utf-8') as f:
        f.write(create_stmt + '\n')
        cur.execute('SELECT {} FROM {}'.format(','.join(columns), tbl_name))
        for row in cur.fetchall():
            num_rows += 1
            f.write('INSERT INTO {} VALUES('.format(tbl_name))
            for i in range(len(row)):
                data_type = column_data[i]['data_type']
                if data_type.lower().startswith('varchar'):
                    # need quotes around value and quoting
                    f.write('\'{}\''.format(nv(row[i], escape_quote=True)))
                else:
                    f.write('{}'.format(nv(row[i])))
                if i < len(row) - 1:
                    f.write(', ')
                else:
                    f.write(');\n')
        f.close()

    print('{} rows written.'.format(num_rows))


def main():
    db = sqlite3.connect('eve.db')

    write_table_dump(db, 'chrAncestries', ['ancestryID', 'ancestryName', 'bloodlineID',
                                           'perception', 'willpower', 'charisma', 'memory',
                                           'intelligence', 'iconID'])
    write_table_dump(db, 'chrBloodlines', ['bloodlineID', 'bloodlineName', 'raceID',
                                           'shipTypeID', 'corporationID', 'perception', 'willpower',
                                           'charisma', 'memory', 'intelligence', 'iconID'])
    write_table_dump(db, 'chrFactions', ['factionID', 'factionName', 'raceIDs',
                                         'solarSystemID', 'corporationID', 'iconID'])
    write_table_dump(db, 'chrRaces', ['raceID', 'raceName', 'iconID'])

    write_table_dump(db, 'invCategories', ['categoryID', 'categoryName', 'iconID'])
    write_table_dump(db, 'invGroups', ['groupID', 'categoryID', 'groupName', 'iconID'])
    write_table_dump(db, 'invTypes', ['typeID', 'groupID', 'typeName', 'iconID', 'graphicID'])

    write_table_dump(db, 'dgmAttributeTypes', ['attributeID', 'attributeName', 'description', 'iconID',
                                               'defaultValue', 'published', 'displayName', 'unitID',
                                               'stackable', 'highIsGood', 'categoryID'])
    write_table_dump(db, 'dgmTypeAttributes', ['typeID', 'attributeID', 'valueInt', 'valueFloat'])


if __name__ == '__main__':
    main()
