# Capture The Flag

Simple multiplayer CTF gameplay using Unreal 5.5 and GAS.

## About the project
I decided to use listen server because when I was trying to setup dedicated server, I discovered that the engine distributed directly from the store didn't include server build support, and I didn't want to waste more time on setup. So instead I just went with listen server that is just there, even though this creates exception cases for the host all the time.

This was also my first time using the Gameplay Ability System (GAS), so I had a lot of learning to do. As for networking, I have worked in multiplayer in Unity using EntitasECS to handle replication and protobuffs for RPCs, and, in Unreal, I have once worked in a small project to try and learn a bit about Unreal's networking, but it was a lot simpler.
And this being my first experience with GAS, I decided to first implement the mechanics as I would without it, also to avoid refactoring the template too early before I really understood how it's all setup. After I had my core mechanics working, I started refactoring them, one by one, into the GAS way. This helped ensure I had the core mechanics working regardless of whether I managed to learn GAS. I think this is a nice incremental way of working when learning something new.  

Except for the flag icon (<a href="https://www.flaticon.com/free-icons/flag" title="flag icons">Flag icons created by Smashicons - Flaticon</a>), all assets I used were from either one of the other templates or the starter content or procedurally made by me (UI materials basically).

About the code quality, I think there is quite a bit of tight coupling in the main module, mostly because the template throws everything together in a single module. It's simple to follow, but it has some coupling. And this tight coupling caused some issues as well, noted bellow.
Some of new code I created, I separated and modularized as much as I could given the huge main module (also bloated Character class). My main focus on this was both time and learning GAS, so I decided not to spend too much time refactoring what was already working.

The animations I imported from the 3rd person template and the Sample Animation Template, retargeted and edited them a bit to make them feel nicer for this.
I also implemented my own materials for post-processing and UI when needed, like the bases' material used to pain the world with the team color or the health bar.

I wanted to add some flavors and comeback chances, so I made that the player holding the flag would have their speed halved. The idea is that the team must protect whoever has the flag and this should also give more of a chance for the other team to steal it back, so it doesn't become too one-sided. But I think that for this to work better, the map also needs to be bigger, as well as team size. I limit to 4 players so I could playtest on the same machine though.

## Timeline
I had 7 days to work on this project. The first day I didn't do much coding, I spent my time setupping the environment and studying the templates and documentation for networking mostly.
From the second day onwards I was working for about 5-6 hours a day, for 6 days, totalling somewhere close to 32h of work on this, which is about 4 days of full time 8h work.

I have some rough breakdown of what I did for (almost) every day.
- The first day I mostly did setup and some research/learning into what I wanted to do.
- Days 2-3: working on the gameplay CTF mechanics, animations, setupping characters for 3rd person view, networking and UI.
- Day 4: gameplay debugging and polishing, started to learn GAS. 
- Day 5: learning about GAS, reading docs, following tutorials and testing some things. Here I started converting what I already had into a more GAS-style way of doing things.
- Day 6: implementing more abilities and effects in GAS, there were a few hiccups, but it all worked eventually. When I finished all core implementation, I built the project and to focus exclusively on bugs (specially in packaged builds)
- Day 7: bug fixing the entire day, and some polishing.

## Known issues
There are some issues that I know of:

Critical:
- There is a bug that I believe is related to projectile replication that causes a hard crash. I couldn't find the cause but the stack trace always come from garbage collector or networking serialization, so I have a hunch this has to do with either destroying an actor that's trying to replicate or some property might be overwritten with null and I couldn't find it.

High priority:
- I don't think my replication is perfectly configured because there's quite a bit of stuttering for client movement, and I understand that the CharacterMovementComponent should already be able to do prediction and reconciliation. But as this was only noticeable in packaged builds, I only notices this too late and I had hard crashes to fix.
- There is no control for match start/reset to check for players' state, there is probably some bugs related to dying at the same time match resets or similar situations.
- There is no main menu, to play the game it must be packaged as development/debug and ran with listen and open IP commands. I included 2 .bat files to make this simpler but it's certainly not ideal.

Medium priority:
- Health bar SDF has no control for aspect ratio, the consequence is that to make a rectangle, currently, you have to stretch the UVs
- I started working on locomotion animation but eventually stopped to focus on more important things (refactoring gameplay with GAS) and this wound up being unused for now.

Low priority:
- Weapon has shadow but character does not.
- There is a strong relation between color and team, probably because I got kind of stuck in the Red/Blue teams, but after a while I realized I should have just made the logic with Team A/B and always assign Blue for player's own team and Red for the enemies'. This would reduce logic and corner cases to handle coloring, could be done fully client side and I think it's a better design, most people already associate red with enemy. 
- There is a name widget that probably shouldn't be used in actual gameplay because it could reveal player's location. The reason I implemented this was to help me know which player was which client (or host) to help with development. Maybe this could be used for spectators only or for same team players.
- When game reaches max players, it still allows new players to join as spectators, but I implemented basically nothing for spectators, it's mostly as a proof of concept.

The two biggest issues for me is the hard crash that I couldn't fix in time and the movement replication issue, as I strongly believe that player control should be the top priority in responsiveness. Aside from these issues, I'm quite happy with all I learned about GAS, even if I didn't manage to implement many different features or effects.
Also, I hate listen servers and all their corner cases, it's a lot simpler to work with dedicated server, if I already had that setup.

One of the biggest issues was actually the changed APIs. I am using unreal 5.5 and there's a lot of API changes for both the engine, and, **especially** GAS since 5.3. This caused me a lot of trouble when reading documentations and tutorials that didn't match what I had. 

## Requirements
- [x] Multiplayer (either listen or dedicated server)
- [x] Upon joining, players are assigned to either blue or red teams
- [x] Flag spawn at the center of the map
- [x] Player can grab the flag by touching
- [x] Player can score points by entering their bases while holding the flag
- [x] When a player dies holding the flag, it drops at the same location
- [x] Upon scoring, flag returns to its spawn position
- [x] Game finishes/resets on 3 points

#### Optional requirements:
- [x] Simple UI for score
- [x] Simple use of GAS
- [ ] More gameplay features (open)
