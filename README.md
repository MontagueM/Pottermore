# Pottermore
Pottermore is a small VR test project in Unreal Engine 4.25 to experiment with Niagara particle effects and motion controller motion. 

I wanted to test out programming the following systems in C++:
- Niagara particle systems
- Complex collision systems
- Spell activation systems
- Multi-spell integrations (eg. blocking an attack spell)

The system is quite basic but fun at its core. Blocking spells by waving your wand across yourself, to then quickly return an attack was great. I think this style of mechanic could work quite well in a dueling game with two VR players.

The main issue was dealing with separating the collision spell system and the niagara systems, as the niagara systems have no public methods or variables. Instead, I synced a projectile with the same velocity and lifetime to mimick the collision as close as possible.

Blocking a spell, then casting one:
![blockspell](https://i.imgur.com/93yQKA2.gif)
