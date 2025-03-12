import json
import os
import os.path
import zipfile

import requests

from google.auth.transport.requests import Request
from google.oauth2.credentials import Credentials
from google_auth_oauthlib.flow import InstalledAppFlow
from googleapiclient.discovery import build
from googleapiclient.errors import HttpError




def GetSpreadsheet():
    creds = None
    # The file token.json stores the user's access and refresh tokens, and is
    # created automatically when the authorization flow completes for the first
    # time.
    if os.path.exists("token.json"):
        creds = Credentials.from_authorized_user_file("token.json", ["https://www.googleapis.com/auth/spreadsheets"])
    # If there are no (valid) credentials available, let the user log in.
    if not creds or not creds.valid:
        if creds and creds.expired and creds.refresh_token:
            creds.refresh(Request())
        else:
            flow = InstalledAppFlow.from_client_secrets_file(
                "google_credentials.json", ["https://www.googleapis.com/auth/spreadsheets"]
            )
            creds = flow.run_local_server(port=0)
        # Save the credentials for the next run
        with open("token.json", "w") as token:
            token.write(creds.to_json())

    service = build("sheets", "v4", credentials=creds)

    # Call the Sheets API
    return service.spreadsheets()

def GetAllGameIds(sheet, spreadsheet_id, current_sheet):
    result = (
        sheet.values()
        .get(spreadsheetId=spreadsheet_id, range=current_sheet + '!' + 'A:A')
        .execute()
    )
    values = result.get("values", [])

    if not values:
      print("No data found.")
      return []
    
    all_game_ids = []
    for i in range(1,len(values)):
        all_game_ids.append(int(values[i][0]))
    
    return all_game_ids

def GetSpreadsheetID():
    f = open("spreadsheetID.txt", "r")
    return (f.readline().rstrip(), f.readline().rstrip(), int(f.readline().rstrip()))

def InsertRow(sheet, sheet_id, sheet_num, data, new_rows):
    string_data = ''
    for elem in data:
        string_data += str(elem) + ';'

    batch_update_spreadsheet_request_body = {
        "requests": [
            {
                "insertRange": {
                    "range": {
                        "sheetId": sheet_num,
                        "startRowIndex": new_rows + 1,
                        "endRowIndex": new_rows + 2
                    },
                    "shiftDimension": "ROWS"
                }
            },
            {
                "pasteData": {
                    "data": string_data,
                    "type": "PASTE_VALUES",
                    "delimiter": ";",
                    "coordinate": {
                        "sheetId": sheet_num,
                        "rowIndex": new_rows + 1
                    }
                }
            }
        ]
    }
    request = sheet.batchUpdate(spreadsheetId=sheet_id, body=batch_update_spreadsheet_request_body)
    response = request.execute()



def GetBotInfo():
    f = open("aiarena_info.txt", "r")
    bot_id = int(f.readline())
    bot_name = f.readline().rstrip()
    token = f.readline().rstrip()
    file_path = f.readline().rstrip()
    user_id = f.readline().rstrip()
    auth = {'Authorization': f'Token {token}'}
    return (bot_id, bot_name, auth, file_path, int(user_id))

def GetMatches(bot_id, auth):
    # requests.get(url).text returns a dictionary formatted as a string and we need dictionaries
    response = requests.get(f'https://aiarena.net/api/match-participations/?bot={bot_id}&ordering=-match', headers=auth)
    assert response.status_code == 200, 'Unexpected status_code returned from match-participations'
    return json.loads(response.text)

def GetMatchData(participation, i, auth, bot_name, bot_id, file_path, user_id):
    response = requests.get(f'https://aiarena.net/api/matches/{participation["results"][i]["match"]}', headers=auth)
    assert response.status_code == 200, 'Unexpected status_code returned from results'

    match_details = json.loads(response.text)
    if match_details['result'] == None or participation['results'][i]['avg_step_time'] == None:
        print(f'No result for match {participation["results"][i]["match"]} - skipping')
        return []

    match_id = match_details['result']['match']
    winner = match_details['result']['winner']
    win = 'Win' if winner == bot_id else ('Tie' if winner == None else 'Loss')
    created = match_details['result']['created'].split('T')
    date = created[0] + ', ' + created[1].split('.')[0]

    total_steps = match_details['result']['game_steps']
    bots = [match_details['result']['bot1_name'], match_details['result']['bot2_name']]

    tags = ''
    for tag in match_details['tags']:
        if (tag['user'] == user_id):
            tags += tag['tag_name'] + ', '

    oponent = bots[1] if bots[0] == bot_name else bots[0]
    game_length = total_steps / 22.4
    game_minutes = game_length // 60
    game_seconds = game_length % 60

    elo_change = participation['results'][i]['resultant_elo'] - participation['results'][i]['starting_elo']
    avg_step = participation['results'][i]['avg_step_time'] * 1000
    cause = 'Game Rules' if participation['results'][i]['result_cause'] == 'game_rules' else ('Crash' if win == 'Loss' else 'Oppenent Crash')
    

    if (win == 'Loss' and cause == 'Crash'):
        # download log and replay
        replay_file = match_details['result']['replay_file']
        if replay_file in (None, 'null'):
            print(f'No replay file for match {participation["results"][i]["match"]} - skipping')
        else:
            replay = requests.get(replay_file)  # don't include auth header - AWS doesn't like it
            with open(os.path.join(file_path, str(participation["results"][i]["match"])+'.SC2Replay'), 'wb') as f:
                f.write(replay.content)
        
        log_file = participation['results'][i]['match_log']
        if log_file in (None, 'null'):
            print(f'No log file for match {participation["results"][i]["match"]} - skipping')
        else:
            log = requests.get(log_file, headers=auth)
            full_path = os.path.join(file_path, str(participation["results"][i]["match"])+'.zip')
            with open(full_path, 'wb') as f:
                f.write(log.content)
            try:
                with zipfile.ZipFile(full_path, 'r') as zip_ref:
                    zip_ref.extractall(file_path)
                if os.path.exists(os.path.join(file_path, 'stderr.log')):
                    os.rename(os.path.join(file_path, 'stderr.log'), os.path.join(file_path, str(participation["results"][i]["match"])+'.log'))
                if os.path.exists(full_path):
                    os.remove(full_path)
            except Exception as e:
                print("cannot unzip log")

        
    
    return [match_id, date, oponent, '=LOOKUP(OFFSET(INDIRECT(ADDRESS(ROW(), COLUMN())), 0, -1),Sheet2!A:A,Sheet2!B:B)', win, '=(COUNTIF(OFFSET(INDIRECT(ADDRESS(ROW(), COLUMN())), 0, -1, 20, 1), "Win")/20)', cause, elo_change, round(avg_step), '0:' + str(int(game_minutes)) + ':' + str(int(game_seconds)), tags]



def main():
    sheet = GetSpreadsheet()
    (spreadsheet_id, current_sheet, sheet_num) = GetSpreadsheetID()
    all_game_ids = GetAllGameIds(sheet, spreadsheet_id, current_sheet)
    (bot_id, bot_name, auth, file_path, user_id) = GetBotInfo()
    participation = GetMatches(bot_id, auth)
    new_rows = 0
    games_matched = 0
    for i in range(0, participation['count']):
        curr_id = participation['results'][i]['match']
        if (curr_id in all_game_ids):
                print('game already recorded ' + str(curr_id))
                games_matched += 1
                if (games_matched >= 10):
                    break
                continue
        
        if (participation['results'][i]['starting_elo'] == None):
            print('no match found for id ' + str(curr_id))
            continue

        data = GetMatchData(participation, i, auth, bot_name, bot_id, file_path, user_id)
        if (data != []):
            # new game
            InsertRow(sheet, spreadsheet_id, sheet_num, data, new_rows)
            new_rows += 1
        else:
            print('no data found for id ' + str(curr_id))

    print('added ' + str(new_rows) + ' games')
    


if __name__ == "__main__":
    main()