import sys
sys.stdout.write("Starting fix...\n")

with open('Spectra-10.40-main/10.40/PlayerBots.h', 'r') as f:
    c = f.read()

c = c.replace('Pawn->IsDead()', 'Pawn->GetHealth() <= 0')
c = c.replace('bot->Pawn->IsDead()', 'bot->Pawn->GetHealth() <= 0')
c = c.replace('!bot->NearestPlayerPawn->IsDead()', 'bot->NearestPlayerPawn->GetHealth() > 0')

with open('Spectra-10.40-main/10.40/PlayerBots.h', 'w') as f:
    f.write(c)

sys.stdout.write("Done!\n")
