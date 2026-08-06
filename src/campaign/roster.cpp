#include "roster.h"
#include <cstdio>
#include <cstring>

namespace tehi {

static const CharacterProfile PROFILES[] = {
    // Protagonist
    {0, "Living_Sin", "Unknown", Branch::Special, "Master Chief Petty Officer", "Operative",
     "A humanoid A.I. construct housed in a black and green Mjolnir-derived armor system. Purpose and past are fragmented; searches for meaning across war-torn worlds. The armor bears the rank insignia of a Master Chief Petty Officer of the Navy, though few remember what that rank actually means.", true},
    // Marines - infantry, breachers, heavy weapons
    {1, "Reaper", "Cole Vasquez", Branch::Marines, "Staff Sergeant", "Infantry Lead",
     "Led a four-man squad through fortified urban terrain, holding a key crossroads for 18 hours against repeated armored probes."},
    {2, "Breacher", "Javi Mendez", Branch::Marines, "Corporal", "Assault",
     "Specialized in explosive entry; cleared twelve strongpoints in a single operation without casualty."},
    {3, "Hammer", "Rory Pike", Branch::Marines, "Sergeant", "Heavy Weapons",
     "Mounted a .50 cal on a disabled APC and held an intersection until resupply arrived."},
    {4, "Rook", "Toby Hines", Branch::Marines, "Lance Corporal", "Rifleman",
     "Provided accurate fire support across 800m, enabling an entire platoon to advance under cover."},
    {5, "Spectre", "Nate Roan", Branch::Marines, "Gunnery Sergeant", "Recon",
     "Infiltrated behind enemy lines for 72 hours, calling in precision strikes with zero detection."},
    {6, "Vortex", "Dane Kross", Branch::Marines, "Corporal", "Auto Rifleman",
     "Maintained continuous suppressive fire during a retrograde maneuver, covering the withdrawal of six wounded."},
    {7, "Axe", "Mack Tully", Branch::Marines, "Sergeant", "Melee / CQB",
     "Cleared three close-quarters rooms in a hostage rescue; neutralized threats at contact distance."},
    {8, "Sentinel", "Will Carver", Branch::Marines, "Lieutenant", "Platoon Commander",
     "Coordinated a multi-squad assault on a fortified ridge; his battle plan minimized casualties and secured the high ground."},
    {9, "Grit", "Eddie Sloan", Branch::Marines, "Private First Class", "Rifleman",
     "Refused evacuation after taking shrapnel, staying on his feet to relay targeting data to close air support."},
    {10, "Forge", "Ray Voss", Branch::Marines, "Master Sergeant", "Armorer / Tech",
     "Kept squad weapons operational under constant fire; performed field repairs that saved three missions."},
    {11, "Striker", "Leo Brandt", Branch::Marines, "Corporal", "Anti-Armor",
     "Destroyed two enemy tanks with shoulder-fired weapons during a delaying action."},
    {12, "Trail", "Sam Bello", Branch::Marines, "Sergeant", "Scout Sniper",
     "Registered 27 confirmed hits at ranges beyond 900m; provided overwatch for seven patrols."},
    {13, "Havoc", "Dom Reese", Branch::Marines, "Lance Corporal", "Assault Amphibian",
     "Drove an Amtrac through surf under fire, landing troops exactly on objective despite mechanical damage."},
    {14, "Ridge", "Cal Dunlap", Branch::Marines, "Captain", "Operations",
     "Planned a 48-hour continuous operation that disrupted an enemy supply network across 200km."},
    {15, "Blitz", "Marty Kade", Branch::Marines, "Corporal", "Infantry",
     "Led a quick reaction force that routed an ambush, recovering four isolated personnel."},

    // Navy - operators, divers, aviators, support
    {16, "Seawolf", "Trent Hale", Branch::Navy, "Lieutenant", "Naval Special Warfare",
     "Conducted a ship-boarding operation in rough seas; secured a hostile vessel and its cargo without diplomatic incident."},
    {17, "Depth", "Owen Marr", Branch::Navy, "Petty Officer", "Diver / EOD",
     "Disarmed an underwater explosive device in a shipping channel, restoring safe navigation within hours."},
    {18, "Talon", "Jake Soler", Branch::Navy, "Commander", "Naval Aviator",
     "Flew close air support through heavy cloud cover, marking targets with pinpoint accuracy for ground troops."},
    {19, "Harbor", "Neil Frost", Branch::Navy, "Lieutenant", "Intelligence",
     "Decoded a maritime threat signal that prevented a port attack; coordinated interagency response in under ninety minutes."},
    {20, "Gulf", "Marco Vain", Branch::Navy, "Captain", "Surface Warfare",
     "Commanded a destroyer during a multi-threat engagement; coordinated layered defense that neutralized incoming strikes."},
    {21, "Reef", "Cal Shore", Branch::Navy, "Petty Officer", " corpsman / Medic",
     "Treated twelve casualties under fire, moving between positions to stabilize the most severely wounded."},
    {22, "Swell", "Ian Drake", Branch::Navy, "Lieutenant", "Helicopter Crew Chief",
     "Managed in-flight repairs during a medevac under fire, keeping the aircraft airborne to the hospital."},
    {23, "Pier", "Dale Rook", Branch::Navy, "Chief Petty Officer", "Boat Crew",
     "Transported a special operations team through mined waters, using sonar to avoid hazards."},
    {24, "Tide", "Rex Hauer", Branch::Navy, "Ensign", "Sonar Technician",
     "Tracked a submerged contact for six hours, maintaining position through evasive maneuvers until reinforcements arrived."},
    {25, "Cove", "Sasha Neer", Branch::Navy, "Petty Officer", "Cryptologic Technician",
     "Intercepted and translated an encrypted threat broadcast, enabling a preemptive defensive posture."},
    {26, "Lighthouse", "Neil Voss", Branch::Navy, "Captain", "Commanding Officer",
     "Directed a task force through contested waters; every vessel under his command returned without loss."},
    {27, "Squall", "Mina Holt", Branch::Navy, "Lieutenant", "Meteorology / Flight Ops",
     "Adjusted mission timing around an incoming storm; assets landed safely despite rapidly deteriorating conditions."},
    {28, "Anchor", "Rory Brock", Branch::Navy, "Chief", "Damage Control",
     "Led firefighting and patching efforts belowdecks after a missile strike; kept the ship seaworthy."},
    {29, "Trawler", "Jed Hale", Branch::Navy, "Petty Officer", "Boatswain",
     "Coordinated line-handling and deck operations during a complex replenishment at sea in heavy weather."},
    {30, "Keel", "Walt Boone", Branch::Navy, "Commander", "Submarine Officer",
     "Executed a silent extraction under ice; navigated a tight channel while avoiding detection."},

    // Air Force - pilots, JTACs, maintainers, space
    {31, "Eagle", "Sky Rourke", Branch::AirForce, "Captain", "Fighter Pilot",
     "Scored three air-to-air engagements in one sortie, protecting the strike package from hostile interceptors."},
    {32, "Maverick", "Drew Fenn", Branch::AirForce, "Major", "Test Pilot",
     "Flew a modified weapons system in its first operational trial, validating performance parameters under live-fire conditions."},
    {33, "Raptor", "Gavin Steele", Branch::AirForce, "Lieutenant Colonel", "Squadron Commander",
     "Led a mixed formation through contested airspace; every aircraft returned with mission success."},
    {34, "Warden", "Cliff Harr", Branch::AirForce, "Senior Airman", "TAC-P / JTAC",
     "Directed 12 close air support missions in one day, adjusting fire patterns to avoid civilian structures."},
    {35, "Comet", "Jesse Vale", Branch::AirForce, "Captain", "Helicopter Pilot",
     "Extracted 22 isolated personnel under direct fire; flew low-level terrain masking to avoid radar."},
    {36, "Vector", "Nora Priest", Branch::AirForce, "Lieutenant", "Air Battle Manager",
     "Managed a complex air picture during a large-scale exercise, deconflicting over 40 aircraft in real time."},
    {37, "Ion", "Reed Lance", Branch::AirForce, "Technical Sergeant", "Aircraft Maintenance",
     "Restored a crippled airframe to flight status in 14 hours, enabling an urgent operational sortie."},
    {38, "Halo", "Cora Lynn", Branch::AirForce, "Major", "Pararescue",
     "Jumped into a remote crash site and stabilized survivors until extraction could reach them."},
    {39, "Orbit", "Jin Rask", Branch::AirForce, "Colonel", "Space Operations",
     "Managed satellite communications during a crisis, preserving command and control across three theaters."},
    {40, "Strato", "Wynn Gale", Branch::AirForce, "Captain", "Weather Officer",
     "Provided target-area forecasts that allowed bombers to strike through a narrow weather window."},
    {41, "Nimbus", "Tara Frost", Branch::AirForce, "Staff Sergeant", "Cyber Systems",
     "Restored network operations after a cyberattack on forward command infrastructure."},
    {42, "Prop", "Hank Doyle", Branch::AirForce, "Master Sergeant", "Propulsion Specialist",
     "Diagnosed and fixed an engine surge issue that had grounded an entire squadron for weeks."},
    {43, "Avion", "Sid Voss", Branch::AirForce, "Lieutenant", "Flight Test Engineer",
     "Instrumented and validated a new electronic warfare suite, writing the tactics manual for its use."},
    {44, "Zenith", "Avery Cole", Branch::AirForce, "General", "Strike Command",
     "Directed a coordinated campaign that disabled key enemy air defenses, enabling follow-on operations."},
    {45, "Compass", "Ellie North", Branch::AirForce, "Major", "Intelligence Analyst",
     "Identified a hidden weapons cache from satellite imagery, leading to a successful precision strike."},
    {46, "Flare", "Kit Bram", Branch::AirForce, "Captain", "Combat Search and Rescue",
     "Located and recovered two isolated aircrew behind enemy lines, coordinating air and ground assets."},
    {47, "Mach", "Ray Zin", Branch::AirForce, "Colonel", "Test Pilot Wing Commander",
     "Pushed a new airframe through its envelope, collecting data that allowed safe operational deployment."},
    {48, "Prism", "Nina Vale", Branch::AirForce, "Lieutenant", "Cryptologic Language Analyst",
     "Translated intercepted communications in real time, revealing an impending coordinated assault."},
    {49, "Aegis", "Hale Boone", Branch::AirForce, "General", "Missile Defense",
     "Designed and fielded an integrated air and missile defense architecture that protected forward areas."},
    {50, "Valkyrie", "Sora Lin", Branch::AirForce, "Captain", "Space Crew Officer",
     "Managed onboard systems during an extended orbital mission; all experiments completed successfully."},
    {51, "Razor", "Finn Hale", Branch::Marines, "Corporal", "Combat Engineer",
     "Cleared 400 meters of obstacles under fire, enabling an entire company to advance."},
    {52, "Wraith", "Kade Mercer", Branch::Marines, "Sergeant", "Force Recon",
     "Infiltrated an island complex via submerged swim; gathered intelligence that shaped the entire campaign plan."},
    {53, "Cipher", "Drew Penn", Branch::Navy, "Lieutenant", "SIGINT",
     "Intercepted a command frequency and relayed live translations to the assault force commander."},
    {54, "Banshee", "Rip Carver", Branch::AirForce, "Major", "Fighter Weapons School",
     "Wrote the advanced tactics syllabus used by every fighter squadron in the theater."},
    {55, "Anvil", "Hugh Lance", Branch::Marines, " Gunnery Sergeant", "Fire Support",
     "Coordinated artillery and air strikes in support of an isolated outpost, preventing its overrun."},
    {56, "Surge", "Cal Reed", Branch::Navy, "Petty Officer", "Electrical Tech",
     "Restored power to a critical radar installation under fire, maintaining early warning coverage."},
    {57, "Meridian", "Clint Vale", Branch::AirForce, "Colonel", "Command Surgeon",
     "Established a forward surgical team that saved 34 lives in the first 48 hours of the campaign."},
    {58, "Outrider", "Jax Rook", Branch::Marines, "Lance Corporal", "Light Armored Recon",
     "Pushed 20km ahead of the main force, reporting enemy dispositions and marking safe passage routes."},
    {59, "Pylon", "Bo Tarp", Branch::AirForce, "Staff Sergeant", "Airfield Operations",
     "Restored runway capability after a precision strike, enabling takeoffs within six hours."},
    {60, "Revenant", "Koda Vance", Branch::Marines, "Sergeant", "Counterintelligence",
     "Uncovered a spy ring inside a forward operating base, preventing classified data from reaching hostile forces."},
};

Roster& Roster::instance() {
    static Roster roster;
    return roster;
}

bool Roster::load() {
    m_profiles.assign(std::begin(PROFILES), std::end(PROFILES));
    for (const auto& p : m_profiles) {
        if (p.is_protagonist) {
            m_protagonist_id = p.id;
            break;
        }
    }
    return true;
}

const std::vector<CharacterProfile>& Roster::all() const {
    return m_profiles;
}

const CharacterProfile* Roster::find_by_callsign(const char* callsign) const {
    for (const auto& p : m_profiles) {
        if (std::strcmp(p.callsign, callsign) == 0) return &p;
    }
    return nullptr;
}

const CharacterProfile* Roster::find_by_id(uint32_t id) const {
    for (const auto& p : m_profiles) {
        if (p.id == id) return &p;
    }
    return nullptr;
}

const CharacterProfile* Roster::get_protagonist() const {
    return find_by_id(m_protagonist_id);
}

} // namespace tehi
