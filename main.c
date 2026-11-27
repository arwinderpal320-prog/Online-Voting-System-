```html
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>CivicPulse — Digital Democracy & Online Voting System</title>
    <!-- Tailwind CSS -->
    <script src="https://cdn.tailwindcss.com"></script>
    <!-- Google Fonts: Inter & JetBrains Mono for cryptographic hashes and telemetry -->
    <link rel="preconnect" href="https://fonts.googleapis.com">
    <link rel="preconnect" href="https://fonts.gstatic.com" crossorigin>
    <link href="https://fonts.googleapis.com/css2?family=Inter:wght@400;500;600;700;800&family=JetBrains+Mono:wght@400;500;600&display=swap" rel="stylesheet">
    <script>
        tailwind.config = {
            theme: {
                extend: {
                    fontFamily: {
                        sans: ['Inter', 'system-ui', 'sans-serif'],
                        mono: ['JetBrains Mono', 'monospace'],
                    },
                    colors: {
                        civic: {
                            50: '#f4f6f9',
                            100: '#e8ecf2',
                            200: '#d0d9e6',
                            800: '#1a273b',
                            900: '#0f172a',
                            950: '#090e1a',
                        },
                        emeraldCivic: {
                            500: '#10b981',
                            600: '#059669',
                        },
                        cobalt: {
                            500: '#2563eb',
                            600: '#1d4ed8',
                            700: '#1e40af',
                        }
                    }
                }
            }
        }
    </script>
    <style>
        /* Subtle editorial paper-grid background texture */
        body {
            background-color: #f8fafc;
            background-image: radial-gradient(#cbd5e1 0.75px, transparent 0.75px);
            background-size: 24px 24px;
            color: #0f172a;
        }
        .ballot-card {
            transition: transform 0.18s cubic-bezier(0.22, 1, 0.36, 1), box-shadow 0.18s ease, border-color 0.18s ease;
        }
        .ballot-card:hover:not(.disabled-card) {
            transform: translateY(-2px);
            box-shadow: 0 12px 24px -8px rgba(15, 23, 42, 0.12);
        }
        .receipt-perforation {
            background-image: radial-gradient(circle at 10px 0, transparent 6px, #ffffff 6.5px);
            background-size: 20px 10px;
            background-repeat: repeat-x;
        }
        /* Custom scrollbar for audit logs */
        ::-webkit-scrollbar {
            width: 6px;
            height: 6px;
        }
        ::-webkit-scrollbar-track {
            background: #f1f5f9;
        }
        ::-webkit-scrollbar-thumb {
            background: #cbd5e1;
            border-radius: 9999px;
        }
        ::-webkit-scrollbar-thumb:hover {
            background: #94a3b8;
        }
    </style>
</head>
<body class="min-h-screen flex flex-col antialiased selection:bg-cobalt-600 selection:text-white">

    <!-- Top Civic Masthead & Navigation -->
    <header class="sticky top-0 z-30 bg-white/95 backdrop-blur-md border-b border-slate-200/90 shadow-xs">
        <div class="max-w-7xl mx-auto px-4 sm:px-6 lg:px-8 h-16 flex items-center justify-between gap-3">
            <!-- Brand Identity -->
            <div class="flex items-center gap-3">
                <div class="w-9 h-9 rounded-xl bg-civic-900 text-white flex items-center justify-center shadow-sm">
                    <!-- Custom Civic Ballot Box SVG Emblem -->
                    <svg class="w-5 h-5" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2.2">
                        <path stroke-linecap="round" stroke-linejoin="round" d="M9 12l2 2 4-4m5.618-4.016A11.955 11.955 0 0112 2.944a11.955 11.955 0 01-8.618 3.04A12.02 12.02 0 003 9c0 5.591 3.824 10.29 9 11.622 5.176-1.332 9-6.03 9-11.622 0-1.042-.133-2.052-.382-3.016z" />
                    </svg>
                </div>
                <div>
                    <div class="flex items-center gap-2">
                        <span class="font-extrabold tracking-tight text-slate-900 text-base sm:text-lg">CivicPulse</span>
                        <span class="hidden sm:inline-flex items-center px-2 py-0.5 rounded-md text-[11px] font-mono font-medium bg-slate-100 text-slate-600 border border-slate-200">
                            SYS-2026.4
                        </span>
                    </div>
                    <p class="text-[11px] text-slate-500 hidden md:block">Verifiable Digital Election & Audit Infrastructure</p>
                </div>
            </div>

            <!-- Live Election Status Pill -->
            <div id="globalStatusBadge" class="flex items-center gap-2 px-3 py-1.5 rounded-full text-xs font-semibold border transition-colors">
                <!-- Populated dynamically -->
            </div>

            <!-- Portal Switcher & Reset -->
            <div class="flex items-center gap-2">
                <div class="inline-flex p-1 rounded-xl bg-slate-100 border border-slate-200/80" role="tablist">
                    <button id="tabVoter" onclick="switchRole('voter')" class="px-3 sm:px-4 py-1.5 rounded-lg text-xs sm:text-sm font-semibold transition-all cursor-pointer">
                        Voter Portal
                    </button>
                    <button id="tabAdmin" onclick="requestAdminAccess()" class="px-3 sm:px-4 py-1.5 rounded-lg text-xs sm:text-sm font-semibold transition-all flex items-center gap-1.5 cursor-pointer">
                        <svg id="adminLockIcon" class="w-3.5 h-3.5" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2">
                            <rect x="3" y="11" width="18" height="11" rx="2" ry="2"></rect>
                            <path d="M7 11V7a5 5 0 0 1 10 0v4"></path>
                        </svg>
                        <span>Admin Command</span>
                    </button>
                </div>
            </div>
        </div>
    </header>

    <!-- Non-Blocking Toast Notification Banner -->
    <div id="toastContainer" class="fixed bottom-5 right-5 z-50 flex flex-col gap-2 max-w-sm w-full pointer-events-none px-4 sm:px-0"></div>

    <!-- Main Content Container -->
    <main class="flex-1 max-w-7xl w-full mx-auto px-4 sm:px-6 lg:px-8 py-6 sm:py-8">

        <!-- ========================================================= -->
        <!-- VIEW 1: VOTER PORTAL                                      -->
        <!-- ========================================================= -->
        <section id="voterView" class="space-y-6">
            <!-- Editorial Election Banner -->
            <div class="bg-white rounded-2xl border border-slate-200/90 p-6 sm:p-8 shadow-xs relative overflow-hidden">
                <div class="absolute top-0 right-0 w-64 h-64 bg-gradient-to-bl from-blue-50/80 via-slate-50/20 to-transparent rounded-bl-full pointer-events-none"></div>
                <div class="relative flex flex-col lg:flex-row lg:items-center lg:justify-between gap-6">
                    <div class="max-w-2xl">
                        <div class="inline-flex items-center gap-2 text-xs font-mono uppercase tracking-wider text-cobalt-600 font-semibold mb-2">
                            <span>General Council & Student Senate Election</span>
                            <span>•</span>
                            <span id="electionDistrictLabel">District 01</span>
                        </div>
                        <h1 class="text-2xl sm:text-3xl font-extrabold tracking-tight text-slate-900">
                            Official Digital Ballot
                        </h1>
                        <p class="mt-2 text-sm sm:text-base text-slate-600 leading-relaxed">
                            Authenticate with your registered Voter ID below to unlock your single-use ballot. Each cast vote generates a tamper-evident cryptographic receipt.
                        </p>
                    </div>
                    <div class="flex flex-wrap sm:flex-nowrap items-center gap-4 bg-slate-50 p-4 rounded-xl border border-slate-200/80 shrink-0">
                        <div>
                            <div class="text-[11px] font-mono uppercase text-slate-500">Registered Voters</div>
                            <div id="heroTotalVoters" class="text-xl font-bold text-slate-900">0</div>
                        </div>
                        <div class="h-8 w-px bg-slate-200"></div>
                        <div>
                            <div class="text-[11px] font-mono uppercase text-slate-500">Ballots Cast</div>
                            <div id="heroVotesCast" class="text-xl font-bold text-cobalt-600">0</div>
                        </div>
                        <div class="h-8 w-px bg-slate-200"></div>
                        <div>
                            <div class="text-[11px] font-mono uppercase text-slate-500">Turnout</div>
                            <div id="heroTurnout" class="text-xl font-bold text-emerald-600">0%</div>
                        </div>
                    </div>
                </div>
            </div>

            <!-- Step 1: Voter Verification Bar -->
            <div class="bg-white rounded-2xl border border-slate-200/90 p-5 sm:p-6 shadow-xs">
                <div class="flex flex-col lg:flex-row lg:items-end justify-between gap-5">
                    <div class="flex-1 max-w-xl">
                        <label for="voterIdInput" class="block text-xs font-bold uppercase tracking-wider text-slate-700 mb-2">
                            Step 1 — Voter Identity Verification
                        </label>
                        <div class="flex flex-col sm:flex-row gap-2.5">
                            <div class="relative flex-1">
                                <input type="text" id="voterIdInput" placeholder="Enter Voter ID (e.g., VOT-101)"
                                    class="w-full pl-4 pr-10 py-3 rounded-xl border border-slate-300 font-mono text-sm uppercase tracking-wider focus:outline-none focus:ring-2 focus:ring-cobalt-600 focus:border-cobalt-600 bg-slate-50/50 transition"
                                    onkeydown="if(event.key === 'Enter') verifyVoter()" />
                                <button onclick="clearVoterSession()" id="clearVoterBtn" title="Clear ID" class="hidden absolute right-3 top-1/2 -translate-y-1/2 text-slate-400 hover:text-slate-600 p-1">
                                    ✕
                                </button>
                            </div>
                            <button onclick="verifyVoter()" class="px-6 py-3 rounded-xl bg-civic-900 hover:bg-civic-800 text-white font-semibold text-sm shadow-xs transition cursor-pointer shrink-0">
                                Verify Eligibility
                            </button>
                        </div>
                    </div>

                    <!-- Quick Fill Demo IDs -->
                    <div class="flex-1">
                        <div class="flex items-center justify-between mb-2">
                            <span class="text-xs font-semibold text-slate-500">Quick Fill Demo IDs (Click to test):</span>
                            <button onclick="openResetModal()" class="text-xs font-medium text-slate-500 hover:text-rose-600 underline underline-offset-2 cursor-pointer">
                                Reset Demo
                            </button>
                        </div>
                        <div id="demoVoterChips" class="flex flex-wrap gap-2">
                            <!-- Rendered dynamically -->
                        </div>
                    </div>
                </div>

                <!-- Dynamic Verification Status Alert Box -->
                <div id="voterStatusAlert" class="hidden mt-5 rounded-xl p-4 border text-sm"></div>
            </div>

            <!-- Step 2: Digital Ballot OR Cryptographic Receipt -->
            <div id="ballotWorkspace">
                <!-- Locked State Placeholder -->
                <div id="ballotLockedState" class="bg-white/70 rounded-2xl border-2 border-dashed border-slate-200 p-10 sm:p-14 text-center">
                    <div class="w-12 h-12 rounded-2xl bg-slate-100 text-slate-500 mx-auto flex items-center justify-center mb-4">
                        <svg class="w-6 h-6" fill="none" stroke="currentColor" stroke-width="2" viewBox="0 0 24 24">
                            <path stroke-linecap="round" stroke-linejoin="round" d="M15 7a2 2 0 012 2m4 0a6 6 0 01-7.743 5.743L11 17H9v2H7v2H4a1 1 0 01-1-1v-2.586a1 1 0 01.293-.707l5.964-5.964A6 6 0 1121 9z" />
                        </svg>
                    </div>
                    <h3 class="text-lg font-bold text-slate-800">Ballot Locked Pending Verification</h3>
                    <p class="text-sm text-slate-500 max-w-md mx-auto mt-1">
                        Select a demo Voter ID chip above or enter a registered ID to inspect candidates and cast your official vote.
                    </p>
                </div>

                <!-- Active Ballot Grid -->
                <div id="activeBallotSection" class="hidden space-y-6">
                    <div class="flex flex-col sm:flex-row sm:items-center justify-between gap-2">
                        <div>
                            <h2 class="text-lg font-bold text-slate-900">Step 2 — Select One Candidate</h2>
                            <p class="text-xs sm:text-sm text-slate-500">Review candidate manifestos carefully. Once submitted, your vote is final and cannot be altered.</p>
                        </div>
                        <span id="verifiedVoterBadge" class="inline-flex items-center gap-2 px-3 py-1.5 rounded-lg bg-emerald-50 border border-emerald-200 text-emerald-800 text-xs font-mono font-semibold self-start sm:self-auto">
                            <!-- Populated dynamically -->
                        </span>
                    </div>

                    <!-- Candidate Cards Grid -->
                    <div id="candidateBallotGrid" class="grid grid-cols-1 md:grid-cols-2 lg:grid-cols-3 gap-5">
                        <!-- Candidate cards rendered via JS -->
                    </div>

                    <!-- Sticky/Bottom Ballot Submission Bar -->
                    <div class="bg-white rounded-2xl border border-slate-200 p-4 sm:p-5 shadow-sm flex flex-col sm:flex-row items-center justify-between gap-4">
                        <div class="flex items-center gap-3 text-sm">
                            <div class="w-2.5 h-2.5 rounded-full bg-cobalt-600 animate-pulse"></div>
                            <span id="selectionSummaryText" class="text-slate-600 font-medium">No candidate selected yet. Click a card above to mark your ballot.</span>
                        </div>
                        <button id="reviewBallotBtn" onclick="openConfirmVoteModal()" disabled
                            class="w-full sm:w-auto px-6 py-3 rounded-xl bg-cobalt-600 hover:bg-cobalt-700 disabled:bg-slate-200 disabled:text-slate-400 disabled:cursor-not-allowed text-white font-semibold text-sm shadow-xs transition cursor-pointer">
                            Review & Cast Official Ballot →
                        </button>
                    </div>
                </div>

                <!-- Verifiable Cryptographic Receipt View (Shown after voting or for already-voted voter) -->
                <div id="receiptSection" class="hidden max-w-2xl mx-auto">
                    <div class="bg-white rounded-2xl border border-slate-200 shadow-md overflow-hidden">
                        <!-- Top Verified Header -->
                        <div class="bg-civic-900 text-white p-6 sm:p-7">
                            <div class="flex items-center justify-between gap-4">
                                <div class="flex items-center gap-3">
                                    <div class="w-10 h-10 rounded-xl bg-emerald-500/20 border border-emerald-400/40 text-emerald-300 flex items-center justify-center">
                                        <svg class="w-6 h-6" fill="none" stroke="currentColor" stroke-width="2.2" viewBox="0 0 24 24">
                                            <path stroke-linecap="round" stroke-linejoin="round" d="M5 13l4 4L19 7" />
                                        </svg>
                                    </div>
                                    <div>
                                        <span class="text-[11px] font-mono uppercase tracking-widest text-emerald-300">Official Cryptographic Proof</span>
                                        <h3 class="text-xl font-bold">Digital Ballot Receipt</h3>
                                    </div>
                                </div>
                                <span class="px-2.5 py-1 rounded-md bg-white/10 font-mono text-xs text-slate-200">VERIFIED</span>
                            </div>
                        </div>

                        <!-- Receipt Body -->
                        <div class="p-6 sm:p-7 space-y-5">
                            <div class="grid grid-cols-1 sm:grid-cols-2 gap-4 text-sm">
                                <div class="p-3.5 rounded-xl bg-slate-50 border border-slate-200/80">
                                    <div class="text-[11px] font-mono uppercase text-slate-500">Voter Identifier</div>
                                    <div id="rcptVoterId" class="font-mono font-bold text-slate-900 mt-0.5">VOT-101</div>
                                </div>
                                <div class="p-3.5 rounded-xl bg-slate-50 border border-slate-200/80">
                                    <div class="text-[11px] font-mono uppercase text-slate-500">Timestamp (UTC/Local)</div>
                                    <div id="rcptTimestamp" class="font-mono font-semibold text-slate-800 mt-0.5 text-xs">--</div>
                                </div>
                            </div>

                            <div class="p-4 rounded-xl bg-blue-50/60 border border-blue-200/80">
                                <div class="text-[11px] font-mono uppercase text-cobalt-700 font-semibold">Recorded Selection</div>
                                <div class="flex items-center justify-between mt-1">
                                    <span id="rcptCandidateName" class="text-base font-bold text-slate-900">--</span>
                                    <span id="rcptCandidateParty" class="text-xs font-medium px-2.5 py-0.5 rounded-full bg-white border border-blue-200 text-cobalt-700">--</span>
                                </div>
                            </div>

                            <div>
                                <div class="flex items-center justify-between mb-1.5">
                                    <span class="text-[11px] font-mono uppercase text-slate-500 font-semibold">SHA-256 Ballot Verification Hash</span>
                                    <span class="text-[11px] text-emerald-600 font-medium">Immutable Ledger Entry</span>
                                </div>
                                <div id="rcptHash" class="p-3.5 rounded-xl bg-slate-900 text-emerald-400 font-mono text-xs break-all select-all leading-relaxed">
                                    0x00000000000000000000000000000000
                                </div>
                            </div>

                            <!-- Receipt Actions -->
                            <div class="pt-2 flex flex-col sm:flex-row gap-3">
                                <button onclick="copyReceiptToClipboard()" class="flex-1 px-4 py-2.5 rounded-xl bg-slate-100 hover:bg-slate-200 text-slate-800 font-semibold text-xs sm:text-sm flex items-center justify-center gap-2 transition cursor-pointer">
                                    <svg class="w-4 h-4" fill="none" stroke="currentColor" stroke-width="2" viewBox="0 0 24 24">
                                        <rect x="9" y="9" width="13" height="13" rx="2" ry="2"></rect>
                                        <path d="M5 15H4a2 2 0 0 1-2-2V4a2 2 0 0 1 2-2h9a2 2 0 0 1 2 2v1"></path>
                                    </svg>
                                    Copy Verification Hash
                                </button>
                                <button onclick="downloadReceiptFile()" class="flex-1 px-4 py-2.5 rounded-xl bg-civic-900 hover:bg-civic-800 text-white font-semibold text-xs sm:text-sm flex items-center justify-center gap-2 transition cursor-pointer">
                                    <svg class="w-4 h-4" fill="none" stroke="currentColor" stroke-width="2" viewBox="0 0 24 24">
                                        <path stroke-linecap="round" stroke-linejoin="round" d="M4 16v1a3 3 0 003 3h10a3 3 0 003-3v-1m-4-4l-4 4m0 0l-4-4m4 4V4" />
                                    </svg>
                                    Download Receipt (.txt)
                                </button>
                            </div>
                        </div>
                    </div>
                </div>
            </div>
        </section>

        <!-- ========================================================= -->
        <!-- VIEW 2: ADMIN COMMAND CENTER                              -->
        <!-- ========================================================= -->
        <section id="adminView" class="hidden space-y-6">
            <!-- Admin Top Control Strip -->
            <div class="bg-civic-900 text-white rounded-2xl p-6 sm:p-7 shadow-sm">
                <div class="flex flex-col lg:flex-row lg:items-center justify-between gap-6">
                    <div>
                        <div class="inline-flex items-center gap-2 text-xs font-mono uppercase tracking-wider text-blue-300 mb-1">
                            <span>Authenticated Session</span>
                            <span>•</span>
                            <span>Election Commission Console</span>
                        </div>
                        <h1 class="text-2xl sm:text-3xl font-extrabold tracking-tight">Admin Command Center</h1>
                        <p class="text-slate-300 text-sm mt-1">Manage election lifecycle, inspect real-time telemetry, register voters, and audit system events.</p>
                    </div>

                    <!-- Election Lifecycle Switcher -->
                    <div class="flex flex-col sm:flex-row items-stretch sm:items-center gap-3">
                        <div class="bg-white/10 p-1.5 rounded-xl border border-white/15 flex items-center gap-1">
                            <button onclick="setElectionStatus('LIVE')" id="btnStatusLive" class="px-3.5 py-2 rounded-lg text-xs font-bold transition cursor-pointer">
                                ● LIVE
                            </button>
                            <button onclick="setElectionStatus('PAUSED')" id="btnStatusPaused" class="px-3.5 py-2 rounded-lg text-xs font-bold transition cursor-pointer">
                                ❚❚ PAUSED
                            </button>
                            <button onclick="setElectionStatus('CLOSED')" id="btnStatusClosed" class="px-3.5 py-2 rounded-lg text-xs font-bold transition cursor-pointer">
                                ■ CLOSED
                            </button>
                        </div>
                        <button onclick="openResetModal()" class="px-4 py-2.5 rounded-xl bg-rose-600/90 hover:bg-rose-600 text-white text-xs font-bold tracking-wide transition cursor-pointer">
                            Reset Election Data
                        </button>
                    </div>
                </div>
            </div>

            <!-- Leading Candidate / Tie Detection Banner -->
            <div id="leaderBanner" class="rounded-2xl border p-5 flex flex-col sm:flex-row sm:items-center justify-between gap-4 bg-white shadow-xs">
                <!-- Dynamically populated -->
            </div>

            <!-- Telemetry & Analytics Row -->
            <div class="grid grid-cols-1 lg:grid-cols-3 gap-6">
                <!-- Left 2 Cols: Real-Time Vote Tally Bar Chart -->
                <div class="lg:col-span-2 bg-white rounded-2xl border border-slate-200/90 p-6 shadow-xs flex flex-col justify-between">
                    <div>
                        <div class="flex items-center justify-between mb-5">
                            <div>
                                <h2 class="text-base font-bold text-slate-900">Real-Time Vote Telemetry</h2>
                                <p class="text-xs text-slate-500">Live distribution across all registered ballot candidates</p>
                            </div>
                            <span id="adminTotalVotesBadge" class="px-3 py-1 rounded-full bg-slate-100 text-slate-700 font-mono text-xs font-semibold">
                                0 Total Votes
                            </span>
                        </div>
                        <div id="tallyBarsContainer" class="space-y-4">
                            <!-- Horizontal progress bars rendered via JS -->
                        </div>
                    </div>
                    <div class="mt-6 pt-4 border-t border-slate-100 flex items-center justify-between text-xs text-slate-500 font-mono">
                        <span>Algorithm: Deterministic Plurality Count</span>
                        <span id="lastTelemetryUpdate">Synced just now</span>
                    </div>
                </div>

                <!-- Right Col: Voter Turnout Ring & Breakdown -->
                <div class="bg-white rounded-2xl border border-slate-200/90 p-6 shadow-xs flex flex-col items-center justify-between text-center">
                    <div class="w-full flex items-center justify-between">
                        <h2 class="text-base font-bold text-slate-900">Voter Turnout</h2>
                        <span class="text-xs font-mono text-slate-400">LIVE RING</span>
                    </div>

                    <!-- SVG Circular Turnout Ring -->
                    <div class="relative w-44 h-44 my-4 flex items-center justify-center">
                        <svg class="w-full h-full -rotate-90" viewBox="0 0 120 120">
                            <circle cx="60" cy="60" r="50" fill="none" stroke="#e2e8f0" stroke-width="11" />
                            <circle id="turnoutRingCircle" cx="60" cy="60" r="50" fill="none" stroke="#2563eb" stroke-width="11"
                                stroke-dasharray="314.16" stroke-dashoffset="314.16" stroke-linecap="round"
                                class="transition-all duration-700 ease-out" />
                        </svg>
                        <div class="absolute inset-0 flex flex-col items-center justify-center">
                            <span id="turnoutRingPercent" class="text-3xl font-extrabold text-slate-900">0%</span>
                            <span class="text-[11px] font-mono uppercase text-slate-500">Participation</span>
                        </div>
                    </div>

                    <div class="w-full grid grid-cols-2 gap-3 pt-2 border-t border-slate-100 text-left">
                        <div class="p-2.5 rounded-xl bg-slate-50">
                            <div class="text-[11px] text-slate-500">Voted</div>
                            <div id="turnoutVotedCount" class="text-base font-bold text-slate-900">0</div>
                        </div>
                        <div class="p-2.5 rounded-xl bg-slate-50">
                            <div class="text-[11px] text-slate-500">Remaining</div>
                            <div id="turnoutPendingCount" class="text-base font-bold text-slate-900">0</div>
                        </div>
                    </div>
                </div>
            </div>

            <!-- Management Grid: Candidates & Voters -->
            <div class="grid grid-cols-1 lg:grid-cols-2 gap-6">
                <!-- Candidate Management Card -->
                <div class="bg-white rounded-2xl border border-slate-200/90 p-6 shadow-xs space-y-5">
                    <div>
                        <h3 class="text-base font-bold text-slate-900">Candidate Roster Management</h3>
                        <p class="text-xs text-slate-500">Add new candidates to the ballot or remove existing entries.</p>
                    </div>

                    <form onsubmit="handleAddCandidate(event)" class="grid grid-cols-1 sm:grid-cols-2 gap-3 bg-slate-50 p-4 rounded-xl border border-slate-200/80">
                        <input type="text" id="newCandName" placeholder="Full Name (e.g., Elena Rostova)" required
                            class="px-3.5 py-2 rounded-lg border border-slate-300 text-sm bg-white focus:outline-none focus:ring-2 focus:ring-cobalt-600" />
                        <input type="text" id="newCandParty" placeholder="Coalition / Party Tag" required
                            class="px-3.5 py-2 rounded-lg border border-slate-300 text-sm bg-white focus:outline-none focus:ring-2 focus:ring-cobalt-600" />
                        <input type="text" id="newCandPlatform" placeholder="Key Manifesto / Platform Summary" required
                            class="sm:col-span-2 px-3.5 py-2 rounded-lg border border-slate-300 text-sm bg-white focus:outline-none focus:ring-2 focus:ring-cobalt-600" />
                        <button type="submit" class="sm:col-span-2 py-2.5 px-4 rounded-lg bg-civic-900 hover:bg-civic-800 text-white text-xs font-bold uppercase tracking-wider transition cursor-pointer">
                            + Add Candidate to Ballot
                        </button>
                    </form>

                    <div id="adminCandidateList" class="divide-y divide-slate-100 max-h-72 overflow-y-auto pr-1">
                        <!-- Rendered via JS -->
                    </div>
                </div>

                <!-- Voter Registry Management Card -->
                <div class="bg-white rounded-2xl border border-slate-200/90 p-6 shadow-xs space-y-5">
                    <div>
                        <h3 class="text-base font-bold text-slate-900">Electoral Roll (Voter Registry)</h3>
                        <p class="text-xs text-slate-500">Register new eligible Voter IDs and monitor ballot completion status.</p>
                    </div>

                    <form onsubmit="handleAddVoter(event)" class="flex flex-col sm:flex-row gap-2.5 bg-slate-50 p-4 rounded-xl border border-slate-200/80">
                        <input type="text" id="newVoterId" placeholder="ID (e.g., VOT-106)" required
                            class="sm:w-36 px-3.5 py-2 rounded-lg border border-slate-300 font-mono text-sm uppercase bg-white focus:outline-none focus:ring-2 focus:ring-cobalt-600" />
                        <input type="text" id="newVoterName" placeholder="Citizen / Student Full Name" required
                            class="flex-1 px-3.5 py-2 rounded-lg border border-slate-300 text-sm bg-white focus:outline-none focus:ring-2 focus:ring-cobalt-600" />
                        <button type="submit" class="py-2.5 px-4 rounded-lg bg-cobalt-600 hover:bg-cobalt-700 text-white text-xs font-bold uppercase tracking-wider transition cursor-pointer shrink-0">
                            + Register
                        </button>
                    </form>

                    <div id="adminVoterList" class="divide-y divide-slate-100 max-h-72 overflow-y-auto pr-1">
                        <!-- Rendered via JS -->
                    </div>
                </div>
            </div>

            <!-- Immutable-Style Audit Log Feed -->
            <div class="bg-white rounded-2xl border border-slate-200/90 p-6 shadow-xs">
                <div class="flex flex-col sm:flex-row sm:items-center justify-between gap-2 mb-4">
                    <div>
                        <h3 class="text-base font-bold text-slate-900 flex items-center gap-2">
                            <span class="w-2 h-2 rounded-full bg-emerald-500"></span>
                            Immutable Election Audit Ledger
                        </h3>
                        <p class="text-xs text-slate-500">Chronological stream of cryptographic ballot submissions and administrative actions.</p>
                    </div>
                    <span id="auditLogCount" class="text-xs font-mono text-slate-500 bg-slate-100 px-2.5 py-1 rounded-md self-start sm:self-auto">
                        0 Events Recorded
                    </span>
                </div>
                <div id="auditLogFeed" class="bg-slate-950 text-slate-300 rounded-xl p-4 font-mono text-xs space-y-2 max-h-64 overflow-y-auto">
                    <!-- Audit logs populated via JS -->
                </div>
            </div>
        </section>
    </main>

    <!-- ========================================================= -->
    <!-- MODAL 1: ADMIN PIN AUTHENTICATION MODAL                   -->
    <!-- ========================================================= -->
    <div id="adminPinModal" class="hidden fixed inset-0 z-50 bg-slate-950/60 backdrop-blur-xs flex items-center justify-center p-4">
        <div class="bg-white rounded-2xl max-w-md w-full p-6 sm:p-7 shadow-xl border border-slate-200 space-y-5">
            <div class="flex items-start justify-between">
                <div class="flex items-center gap-3">
                    <div class="w-10 h-10 rounded-xl bg-civic-900 text-white flex items-center justify-center">
                        <svg class="w-5 h-5" fill="none" stroke="currentColor" stroke-width="2" viewBox="0 0 24 24">
                            <rect x="3" y="11" width="18" height="11" rx="2" ry="2"></rect>
                            <path d="M7 11V7a5 5 0 0 1 10 0v4"></path>
                        </svg>
                    </div>
                    <div>
                        <h3 class="text-lg font-bold text-slate-900">Admin Command Access</h3>
                        <p class="text-xs text-slate-500">Enter Election Officer Security PIN</p>
                    </div>
                </div>
                <button onclick="closeAdminPinModal()" class="text-slate-400 hover:text-slate-600 p-1 cursor-pointer">✕</button>
            </div>

            <div class="p-3 rounded-xl bg-blue-50 border border-blue-200/80 flex items-center justify-between text-xs text-cobalt-700">
                <span>Demo Credentials Hint: PIN is <strong class="font-mono">1234</strong></span>
                <button type="button" onclick="quickDemoAdminLogin()" class="px-2.5 py-1 rounded-lg bg-cobalt-600 text-white font-semibold hover:bg-cobalt-700 transition cursor-pointer">
                    Quick Demo Login
                </button>
            </div>

            <div>
                <label for="adminPinInput" class="block text-xs font-semibold text-slate-700 mb-1.5">4-Digit Security PIN</label>
                <input type="password" id="adminPinInput" maxlength="8" placeholder="••••"
                    class="w-full px-4 py-3 rounded-xl border border-slate-300 font-mono text-center text-xl tracking-[0.5em] focus:outline-none focus:ring-2 focus:ring-cobalt-600"
                    onkeydown="if(event.key === 'Enter') verifyAdminPin()" />
                <p id="adminPinError" class="hidden text-xs text-rose-600 font-medium mt-1.5">Incorrect PIN. Try 1234 for demo access.</p>
            </div>

            <div class="flex gap-3 pt-1">
                <button onclick="closeAdminPinModal()" class="flex-1 py-2.5 rounded-xl border border-slate-200 text-slate-700 font-semibold text-sm hover:bg-slate-50 transition cursor-pointer">
                    Cancel
                </button>
                <button onclick="verifyAdminPin()" class="flex-1 py-2.5 rounded-xl bg-civic-900 text-white font-semibold text-sm hover:bg-civic-800 transition cursor-pointer">
                    Unlock Console
                </button>
            </div>
        </div>
    </div>

    <!-- ========================================================= -->
    <!-- MODAL 2: CONFIRM OFFICIAL BALLOT SUBMISSION               -->
    <!-- ========================================================= -->
    <div id="confirmVoteModal" class="hidden fixed inset-0 z-50 bg-slate-950/60 backdrop-blur-xs flex items-center justify-center p-4">
        <div class="bg-white rounded-2xl max-w-md w-full p-6 sm:p-7 shadow-xl border border-slate-200 space-y-5">
            <div>
                <span class="text-[11px] font-mono uppercase tracking-wider text-cobalt-600 font-bold">Final Ballot Verification</span>
                <h3 class="text-xl font-bold text-slate-900 mt-1">Confirm Your Official Vote</h3>
                <p class="text-xs sm:text-sm text-slate-600 mt-1">
                    You are about to cast your single irreversible ballot for:
                </p>
            </div>

            <div id="confirmCandidateSummary" class="p-4 rounded-xl bg-slate-50 border border-slate-200 flex items-center gap-3.5">
                <!-- Populated dynamically -->
            </div>

            <div class="p-3 rounded-xl bg-amber-50 border border-amber-200 text-xs text-amber-800 leading-relaxed">
                <strong>Notice:</strong> Once submitted, Voter ID <span id="confirmVoterIdText" class="font-mono font-bold"></span> will be marked as voted and locked against duplicate submissions.
            </div>

            <div class="flex gap-3 pt-1">
                <button onclick="closeConfirmVoteModal()" class="flex-1 py-2.5 rounded-xl border border-slate-200 text-slate-700 font-semibold text-sm hover:bg-slate-50 transition cursor-pointer">
                    Change Selection
                </button>
                <button onclick="submitOfficialVote()" class="flex-1 py-2.5 rounded-xl bg-emerald-600 hover:bg-emerald-700 text-white font-semibold text-sm shadow-xs transition cursor-pointer">
                    Confirm & Cast Vote
                </button>
            </div>
        </div>
    </div>

    <!-- ========================================================= -->
    <!-- MODAL 3: RESET DEMO DATA CONFIRMATION                     -->
    <!-- ========================================================= -->
    <div id="resetDemoModal" class="hidden fixed inset-0 z-50 bg-slate-950/60 backdrop-blur-xs flex items-center justify-center p-4">
        <div class="bg-white rounded-2xl max-w-md w-full p-6 shadow-xl border border-slate-200 space-y-4">
            <h3 class="text-lg font-bold text-slate-900">Reset Election Demo Data?</h3>
            <p class="text-sm text-slate-600">
                This will restore the default candidates, reset all Voter IDs (<span class="font-mono text-xs">VOT-101</span> to <span class="font-mono text-xs">VOT-105</span>) to unvoted status, set the election to <strong>LIVE</strong>, and clear custom audit entries.
            </p>
            <div class="flex gap-3 pt-2">
                <button onclick="closeResetModal()" class="flex-1 py-2.5 rounded-xl border border-slate-200 text-slate-700 font-semibold text-sm hover:bg-slate-50 cursor-pointer">
                    Cancel
                </button>
                <button onclick="executeDemoReset()" class="flex-1 py-2.5 rounded-xl bg-rose-600 hover:bg-rose-700 text-white font-semibold text-sm cursor-pointer">
                    Yes, Reset Everything
                </button>
            </div>
        </div>
    </div>

    <!-- ========================================================= -->
    <!-- APPLICATION LOGIC & STATE PERSISTENCE                     -->
    <!-- ========================================================= -->
    <script>
        const STORAGE_KEY = 'civicpulse_voting_system_v1';
        const ADMIN_PIN = '1234';

        // Default Initial Seed Data
        function getDefaultState() {
            return {
                electionStatus: 'LIVE', // 'LIVE' | 'PAUSED' | 'CLOSED'
                candidates: [
                    {
                        id: 'CAND-01',
                        name: 'Dr. Aarav Mehta',
                        party: 'Civic Innovation Alliance',
                        platform: 'Open-source campus infrastructure, transparent budget dashboards, and renewable microgrids.',
                        color: '#2563eb',
                        votes: 1
                    },
                    {
                        id: 'CAND-02',
                        name: 'Maya Lin-Subramanian',
                        party: 'Student Equity & Access',
                        platform: '24/7 digital library access, expanded mental health grants, and zero-fee lab workshops.',
                        color: '#059669',
                        votes: 0
                    },
                    {
                        id: 'CAND-03',
                        name: 'Marcus Vance',
                        party: 'Constitutional Reform Bloc',
                        platform: 'Decentralized department governance, quarterly town-hall referendums, and audit transparency.',
                        color: '#7c3aed',
                        votes: 0
                    }
                ],
                voters: [
                    { id: 'VOT-101', name: 'Ananya Sharma', hasVoted: false, receipt: null },
                    { id: 'VOT-102', name: 'Rohan Kulkarni', hasVoted: false, receipt: null },
                    { id: 'VOT-103', name: 'Priya Nair', hasVoted: false, receipt: null },
                    { id: 'VOT-104', name: 'Siddharth Verma', hasVoted: false, receipt: null },
                    {
                        id: 'VOT-105',
                        name: 'Kavya Menon',
                        hasVoted: true,
                        receipt: {
                            voterId: 'VOT-105',
                            candidateName: 'Dr. Aarav Mehta',
                            candidateParty: 'Civic Innovation Alliance',
                            timestamp: new Date(Date.now() - 3600 * 1000).toISOString().replace('T', ' ').substring(0, 19) + ' UTC',
                            hash: '0x9f86d081884c7d659a2feaa0c55ad015a3bf4f1b2b0b822cd15d6c15b0f00a08'
                        }
                    }
                ],
                auditLogs: [
                    {
                        time: new Date(Date.now() - 7200 * 1000).toLocaleTimeString([], { hour12: false }),
                        type: 'SYSTEM',
                        message: 'Election ledger initialized with 3 certified candidates and 5 registered voters.'
                    },
                    {
                        time: new Date(Date.now() - 3600 * 1000).toLocaleTimeString([], { hour12: false }),
                        type: 'BALLOT',
                        message: 'Anonymized ballot verified & committed [Hash: 0x9f86d0...0a08].'
                    }
                ]
            };
        }

        // Application Runtime State
        let appState = loadState();
        let activeRole = 'voter'; // 'voter' | 'admin'
        let isAdminAuthenticated = false;
        let currentVerifiedVoterId = null;
        let selectedCandidateId = null;

        // Load from localStorage safely
        function loadState() {
            try {
                const raw = localStorage.getItem(STORAGE_KEY);
                if (!raw) return getDefaultState();
                const parsed = JSON.parse(raw);
                if (!parsed || !Array.isArray(parsed.candidates) || !Array.isArray(parsed.voters)) {
                    return getDefaultState();
                }
                return parsed;
            } catch (err) {
                console.error('Failed to read localStorage:', err);
                return getDefaultState();
            }
        }

        // Save to localStorage
        function saveState() {
            try {
                localStorage.setItem(STORAGE_KEY, JSON.stringify(appState));
            } catch (err) {
                showToast('Storage warning: Could not persist data locally.', 'error');
            }
        }

        // Deterministic Pseudo-Cryptographic SHA-256 Style Hash Generator
        function generateBallotHash(voterId, candidateId, timestamp) {
            const seed = `${voterId}::${candidateId}::${timestamp}::${Math.random().toString(36).substring(2)}`;
            let h1 = 0xdeadbeef, h2 = 0x41c6ce57;
            for (let i = 0; i < seed.length; i++) {
                const ch = seed.charCodeAt(i);
                h1 = Math.imul(h1 ^ ch, 2654435761);
                h2 = Math.imul(h2 ^ ch, 1597334677);
            }
            h1 = Math.imul(h1 ^ (h1 >>> 16), 2246822507) ^ Math.imul(h2 ^ (h2 >>> 13), 3266489909);
            h2 = Math.imul(h2 ^ (h2 >>> 16), 2246822507) ^ Math.imul(h1 ^ (h1 >>> 13), 3266489909);
            const partA = (h1 >>> 0).toString(16).padStart(8, '0');
            const partB = (h2 >>> 0).toString(16).padStart(8, '0');
            const partC = ((h1 ^ h2) >>> 0).toString(16).padStart(8, '0');
            const partD = ((h1 + h2) >>> 0).toString(16).padStart(8, '0');
            return `0x${partA}${partB}${partC}${partD}${partB}${partA}${partD}${partC}`;
        }

        // Append Entry to Audit Log
        function logAuditEvent(type, message) {
            const now = new Date().toLocaleTimeString([], { hour12: false });
            appState.auditLogs.unshift({ time: now, type, message });
            if (appState.auditLogs.length > 50) appState.auditLogs.pop();
        }

        // Generate Custom SVG Monogram Avatar for Candidate
        function createCandidateAvatarSVG(name, color) {
            const initials = name
                .replace(/^Dr\.\s+/i, '')
                .split(' ')
                .map(n => n[0])
                .slice(0, 2)
                .join('')
                .toUpperCase();

            return `
                <svg class="w-12 h-12 rounded-xl shrink-0 shadow-xs" viewBox="0 0 48 48" fill="none" xmlns="http://www.w3.org/2000/svg">
                    <rect width="48" height="48" rx="12" fill="${color}" fill-opacity="0.12"/>
                    <rect x="1" y="1" width="46" height="46" rx="11" stroke="${color}" stroke-opacity="0.3" stroke-width="1.5"/>
                    <circle cx="38" cy="10" r="14" fill="${color}" fill-opacity="0.08"/>
                    <text x="50%" y="53%" dominant-baseline="middle" text-anchor="middle" fill="${color}" font-family="Inter, sans-serif" font-weight="800" font-size="16" letter-spacing="0.5">
                        ${initials}
                    </text>
                </svg>
            `;
        }

        // Toast Notification System (Replaces alert())
        function showToast(message, variant = 'info') {
            const container = document.getElementById('toastContainer');
            const toast = document.createElement('div');
            const styles = {
                info: 'bg-civic-900 text-white border-slate-700',
                success: 'bg-emerald-700 text-white border-emerald-600',
                error: 'bg-rose-700 text-white border-rose-600',
                warning: 'bg-amber-600 text-white border-amber-500'
            };
            toast.className = `pointer-events-auto px-4 py-3 rounded-xl border shadow-lg text-xs sm:text-sm font-medium flex items-center justify-between gap-3 transition-all duration-300 ${styles[variant] || styles.info}`;
            toast.innerHTML = `
                <span>${message}</span>
                <button onclick="this.parentElement.remove()" class="opacity-75 hover:opacity-100 text-xs font-bold cursor-pointer">✕</button>
            `;
            container.appendChild(toast);
            setTimeout(() => {
                if (toast.parentElement) toast.remove();
            }, 4000);
        }

        // =========================================================
        // ROLE SWITCHING & ADMIN AUTHENTICATION
        // =========================================================
        function switchRole(role) {
            activeRole = role;
            const voterView = document.getElementById('voterView');
            const adminView = document.getElementById('adminView');
            const tabVoter = document.getElementById('tabVoter');
            const tabAdmin = document.getElementById('tabAdmin');

            if (role === 'voter') {
                voterView.classList.remove('hidden');
                adminView.classList.add('hidden');
                tabVoter.className = 'px-3 sm:px-4 py-1.5 rounded-lg text-xs sm:text-sm font-semibold bg-white text-slate-900 shadow-xs transition-all cursor-pointer';
                tabAdmin.className = 'px-3 sm:px-4 py-1.5 rounded-lg text-xs sm:text-sm font-semibold text-slate-600 hover:text-slate-900 transition-all flex items-center gap-1.5 cursor-pointer';
                renderVoterPortal();
            } else {
                voterView.classList.add('hidden');
                adminView.classList.remove('hidden');
                tabAdmin.className = 'px-3 sm:px-4 py-1.5 rounded-lg text-xs sm:text-sm font-semibold bg-civic-900 text-white shadow-xs transition-all flex items-center gap-1.5 cursor-pointer';
                tabVoter.className = 'px-3 sm:px-4 py-1.5 rounded-lg text-xs sm:text-sm font-semibold text-slate-600 hover:text-slate-900 transition-all cursor-pointer';
                renderAdminDashboard();
            }
        }

        function requestAdminAccess() {
            if (isAdminAuthenticated) {
                switchRole('admin');
                return;
            }
            document.getElementById('adminPinInput').value = '';
            document.getElementById('adminPinError').classList.add('hidden');
            document.getElementById('adminPinModal').classList.remove('hidden');
            setTimeout(() => document.getElementById('adminPinInput').focus(), 50);
        }

        function closeAdminPinModal() {
            document.getElementById('adminPinModal').classList.add('hidden');
        }

        function verifyAdminPin() {
            const val = document.getElementById('adminPinInput').value.trim();
            if (val === ADMIN_PIN) {
                isAdminAuthenticated = true;
                document.getElementById('adminLockIcon').innerHTML = '<path stroke-linecap="round" stroke-linejoin="round" d="M8 11V7a4 4 0 118 0m-4 8v2m-6 4h12a2 2 0 002-2v-6a2 2 0 00-2-2H6a2 2 0 00-2 2v6a2 2 0 002 2z" />';
                closeAdminPinModal();
                logAuditEvent('ADMIN', 'Admin Command Center unlocked via security PIN.');
                saveState();
                switchRole('admin');
                showToast('Authenticated as Election Administrator.', 'success');
            } else {
                document.getElementById('adminPinError').classList.remove('hidden');
            }
        }

        function quickDemoAdminLogin() {
            document.getElementById('adminPinInput').value = ADMIN_PIN;
            verifyAdminPin();
        }

        // =========================================================
        // GLOBAL STATUS & STATS RENDERING
        // =========================================================
        function renderGlobalHeader() {
            const badge = document.getElementById('globalStatusBadge');
            const status = appState.electionStatus;

            if (status === 'LIVE') {
                badge.className = 'flex items-center gap-2 px-3 py-1.5 rounded-full text-xs font-semibold border bg-emerald-50 border-emerald-200 text-emerald-800';
                badge.innerHTML = `<span class="w-2 h-2 rounded-full bg-emerald-500 animate-ping"></span><span>POLLS LIVE</span>`;
            } else if (status === 'PAUSED') {
                badge.className = 'flex items-center gap-2 px-3 py-1.5 rounded-full text-xs font-semibold border bg-amber-50 border-amber-200 text-amber-800';
                badge.innerHTML = `<span class="w-2 h-2 rounded-full bg-amber-500"></span><span>POLLS PAUSED</span>`;
            } else {
                badge.className = 'flex items-center gap-2 px-3 py-1.5 rounded-full text-xs font-semibold border bg-rose-50 border-rose-200 text-rose-800';
                badge.innerHTML = `<span class="w-2 h-2 rounded-full bg-rose-500"></span><span>POLLS CLOSED</span>`;
            }
        }

        // =========================================================
        // VOTER PORTAL LOGIC
        // =========================================================
        function renderVoterPortal() {
            renderGlobalHeader();

            // Update hero stats
            const totalVoters = appState.voters.length;
            const votedCount = appState.voters.filter(v => v.hasVoted).length;
            const turnout = totalVoters > 0 ? Math.round((votedCount / totalVoters) * 100) : 0;

            document.getElementById('heroTotalVoters').textContent = totalVoters;
            document.getElementById('heroVotesCast').textContent = votedCount;
            document.getElementById('heroTurnout').textContent = `${turnout}%`;

            // Render Quick Fill Demo ID Chips
            const chipsContainer = document.getElementById('demoVoterChips');
            chipsContainer.innerHTML = appState.voters.map(v => {
                const isSelected = currentVerifiedVoterId === v.id;
                const statusDot = v.hasVoted
                    ? '<span class="w-1.5 h-1.5 rounded-full bg-slate-400" title="Already Voted"></span>'
                    : '<span class="w-1.5 h-1.5 rounded-full bg-emerald-500" title="Eligible"></span>';
                const activeClass = isSelected
                    ? 'bg-civic-900 text-white border-civic-900'
                    : v.hasVoted
                        ? 'bg-slate-100 text-slate-500 border-slate-200 hover:bg-slate-200/70'
                        : 'bg-white text-slate-700 border-slate-300 hover:border-cobalt-600 hover:text-cobalt-600';

                return `
                    <button onclick="quickFillVoter('${v.id}')" class="inline-flex items-center gap-1.5 px-2.5 py-1 rounded-lg border font-mono text-xs font-medium transition cursor-pointer ${activeClass}">
                        ${statusDot}
                        <span>${v.id}</span>
                    </button>
                `;
            }).join('');

            // Re-evaluate current session if active
            if (currentVerifiedVoterId) {
                const voter = appState.voters.find(v => v.id === currentVerifiedVoterId);
                if (voter) {
                    applyVerifiedVoterUI(voter);
                } else {
                    clearVoterSession();
                }
            }
        }

        function quickFillVoter(id) {
            document.getElementById('voterIdInput').value = id;
            verifyVoter();
        }

        function clearVoterSession() {
            currentVerifiedVoterId = null;
            selectedCandidateId = null;
            document.getElementById('voterIdInput').value = '';
            document.getElementById('clearVoterBtn').classList.add('hidden');
            document.getElementById('voterStatusAlert').classList.add('hidden');
            document.getElementById('ballotLockedState').classList.remove('hidden');
            document.getElementById('activeBallotSection').classList.add('hidden');
            document.getElementById('receiptSection').classList.add('hidden');
            renderVoterPortal();
        }

        function verifyVoter() {
            const rawId = document.getElementById('voterIdInput').value.trim().toUpperCase();
            const alertBox = document.getElementById('voterStatusAlert');

            if (!rawId) {
                alertBox.className = 'mt-5 rounded-xl p-4 border text-sm bg-amber-50 border-amber-200 text-amber-800';
                alertBox.innerHTML = '<strong>Input Required:</strong> Please enter a valid Voter ID or select one of the demo chips.';
                alertBox.classList.remove('hidden');
                return;
            }

            const voter = appState.voters.find(v => v.id.toUpperCase() === rawId);
            document.getElementById('clearVoterBtn').classList.remove('hidden');

            if (!voter) {
                currentVerifiedVoterId = null;
                selectedCandidateId = null;
                alertBox.className = 'mt-5 rounded-xl p-4 border text-sm bg-rose-50 border-rose-200 text-rose-800 flex items-center justify-between';
                alertBox.innerHTML = `<span><strong>Unrecognized Voter ID:</strong> <code class="font-mono font-bold">${rawId}</code> is not found on the electoral roll. Register it in Admin Command or use a demo ID.</span>`;
                alertBox.classList.remove('hidden');
                document.getElementById('ballotLockedState').classList.remove('hidden');
                document.getElementById('activeBallotSection').classList.add('hidden');
                document.getElementById('receiptSection').classList.add('hidden');
                renderVoterPortal();
                return;
            }

            currentVerifiedVoterId = voter.id;
            selectedCandidateId = null;
            renderVoterPortal();
        }

        function applyVerifiedVoterUI(voter) {
            const alertBox = document.getElementById('voterStatusAlert');
            const lockedState = document.getElementById('ballotLockedState');
            const activeBallot = document.getElementById('activeBallotSection');
            const receiptSection = document.getElementById('receiptSection');

            lockedState.classList.add('hidden');

            // Case 1: Duplicate Vote Prevention — Voter Already Voted
            if (voter.hasVoted) {
                alertBox.className = 'mt-5 rounded-xl p-4 border text-sm bg-slate-100 border-slate-300 text-slate-800 flex flex-col sm:flex-row sm:items-center justify-between gap-2';
                alertBox.innerHTML = `
                    <div>
                        <span class="inline-flex items-center gap-1.5 font-bold text-slate-900">
                            <span class="w-2 h-2 rounded-full bg-cobalt-600"></span>
                            Ballot Already Recorded for ${voter.name} (${voter.id})
                        </span>
                        <p class="text-xs text-slate-600 mt-0.5">Duplicate voting is strictly blocked by the election ledger. Your official receipt is displayed below.</p>
                    </div>
                    <span class="px-2.5 py-1 rounded-md bg-white border border-slate-200 font-mono text-xs font-semibold text-slate-700 self-start sm:self-center">STATUS: VOTED</span>
                `;
                alertBox.classList.remove('hidden');
                activeBallot.classList.add('hidden');
                receiptSection.classList.remove('hidden');
                renderReceipt(voter.receipt);
                return;
            }

            // Case 2: Polls are Paused or Closed
            if (appState.electionStatus !== 'LIVE') {
                alertBox.className = 'mt-5 rounded-xl p-4 border text-sm bg-amber-50 border-amber-200 text-amber-900';
                alertBox.innerHTML = `<strong>Voting Currently Suspended:</strong> Hello <strong>${voter.name}</strong> (${voter.id}). Your ID is valid, but polls are currently <strong>${appState.electionStatus}</strong> by the Election Commission.`;
                alertBox.classList.remove('hidden');
                activeBallot.classList.add('hidden');
                receiptSection.classList.add('hidden');
                return;
            }

            // Case 3: Eligible & Polls Live -> Unlock Ballot
            alertBox.className = 'mt-5 rounded-xl p-4 border text-sm bg-emerald-50 border-emerald-200 text-emerald-900 flex items-center justify-between';
            alertBox.innerHTML = `
                <span><strong>Identity Verified:</strong> Welcome, <strong>${voter.name}</strong>. You are eligible to cast 1 official ballot.</span>
                <span class="hidden sm:inline-block font-mono text-xs font-bold text-emerald-700">ELIGIBLE</span>
            `;
            alertBox.classList.remove('hidden');
            receiptSection.classList.add('hidden');
            activeBallot.classList.remove('hidden');

            document.getElementById('verifiedVoterBadge').textContent = `VOTER: ${voter.id} • ${voter.name.toUpperCase()}`;
            renderCandidateBallotCards();
        }

        function renderCandidateBallotCards() {
            const grid = document.getElementById('candidateBallotGrid');
            if (appState.candidates.length === 0) {
                grid.innerHTML = `<div class="col-span-full p-8 text-center bg-white rounded-2xl border border-slate-200 text-slate-500 text-sm">No candidates registered on the ballot yet.</div>`;
                return;
            }

            grid.innerHTML = appState.candidates.map((cand, idx) => {
                const isSelected = selectedCandidateId === cand.id;
                const cardBorder = isSelected
                    ? 'border-cobalt-600 ring-2 ring-cobalt-600/20 bg-blue-50/25'
                    : 'border-slate-200/90 bg-white hover:border-slate-300';

                return `
                    <div onclick="selectCandidate('${cand.id}')"
                         class="ballot-card rounded-2xl border-2 p-5 sm:p-6 cursor-pointer flex flex-col justify-between select-none ${cardBorder}">
                        <div>
                            <div class="flex items-start justify-between gap-3 mb-4">
                                <div class="flex items-center gap-3.5">
                                    ${createCandidateAvatarSVG(cand.name, cand.color)}
                                    <div>
                                        <span class="text-[11px] font-mono text-slate-400">BALLOT #0${idx + 1}</span>
                                        <h3 class="text-base sm:text-lg font-bold text-slate-900 leading-snug">${cand.name}</h3>
                                    </div>
                                </div>
                                <!-- Tactile Radio Indicator -->
                                <div class="w-6 h-6 rounded-full border-2 flex items-center justify-center shrink-0 transition-colors ${isSelected ? 'border-cobalt-600 bg-cobalt-600 text-white' : 'border-slate-300 bg-white'}">
                                    ${isSelected ? '<svg class="w-3.5 h-3.5" fill="none" stroke="currentColor" stroke-width="3" viewBox="0 0 24 24"><path stroke-linecap="round" stroke-linejoin="round" d="M5 13l4 4L19 7"/></svg>' : ''}
                                </div>
                            </div>

                            <div class="mb-3">
                                <span class="inline-block px-2.5 py-1 rounded-md text-xs font-semibold bg-slate-100 text-slate-700 border border-slate-200/70">
                                    ${cand.party}
                                </span>
                            </div>

                            <p class="text-xs sm:text-sm text-slate-600 leading-relaxed">
                                ${cand.platform}
                            </p>
                        </div>

                        <div class="mt-5 pt-3.5 border-t border-slate-100 flex items-center justify-between text-xs font-semibold">
                            <span class="${isSelected ? 'text-cobalt-700' : 'text-slate-400'}">
                                ${isSelected ? '✓ Selected on Ballot' : 'Click to Select'}
                            </span>
                            <span class="font-mono text-[11px] text-slate-400">${cand.id}</span>
                        </div>
                    </div>
                `;
            }).join('');

            updateSelectionBar();
        }

        function selectCandidate(candId) {
            selectedCandidateId = candId;
            renderCandidateBallotCards();
        }

        function updateSelectionBar() {
            const summaryText = document.getElementById('selectionSummaryText');
            const reviewBtn = document.getElementById('reviewBallotBtn');
            const cand = appState.candidates.find(c => c.id === selectedCandidateId);

            if (cand) {
                summaryText.innerHTML = `Selected Candidate: <strong class="text-slate-900">${cand.name}</strong> <span class="text-slate-400">(${cand.party})</span>`;
                reviewBtn.disabled = false;
            } else {
                summaryText.textContent = 'No candidate selected yet. Click a card above to mark your ballot.';
                reviewBtn.disabled = true;
            }
        }

        function openConfirmVoteModal() {
            const voter = appState.voters.find(v => v.id === currentVerifiedVoterId);
            const cand = appState.candidates.find(c => c.id === selectedCandidateId);
            if (!voter || !cand) return;

            document.getElementById('confirmVoterIdText').textContent = voter.id;
            document.getElementById('confirmCandidateSummary').innerHTML = `
                ${createCandidateAvatarSVG(cand.name, cand.color)}
                <div>
                    <div class="text-base font-bold text-slate-900">${cand.name}</div>
                    <div class="text-xs font-medium text-slate-500">${cand.party} • ${cand.id}</div>
                </div>
            `;
            document.getElementById('confirmVoteModal').classList.remove('hidden');
        }

        function closeConfirmVoteModal() {
            document.getElementById('confirmVoteModal').classList.add('hidden');
        }

        function submitOfficialVote() {
            closeConfirmVoteModal();

            if (appState.electionStatus !== 'LIVE') {
                showToast('Cannot cast vote: Polls are no longer live.', 'error');
                renderVoterPortal();
                return;
            }

            const voter = appState.voters.find(v => v.id === currentVerifiedVoterId);
            const cand = appState.candidates.find(c => c.id === selectedCandidateId);

            if (!voter || !cand) return;
            if (voter.hasVoted) {
                showToast('Duplicate vote blocked: This Voter ID has already voted.', 'error');
                renderVoterPortal();
                return;
            }

            const timestamp = new Date().toISOString().replace('T', ' ').substring(0, 19) + ' UTC';
            const hash = generateBallotHash(voter.id, cand.id, timestamp);

            // Commit vote atomically
            cand.votes += 1;
            voter.hasVoted = true;
            voter.receipt = {
                voterId: voter.id,
                candidateName: cand.name,
                candidateParty: cand.party,
                timestamp,
                hash
            };

            logAuditEvent('BALLOT', `Anonymized ballot verified & committed [Hash: ${hash.substring(0, 10)}...${hash.slice(-4)}].`);
            saveState();

            showToast('Official vote recorded! Cryptographic receipt generated.', 'success');
            renderVoterPortal();
        }

        function renderReceipt(receipt) {
            if (!receipt) return;
            document.getElementById('rcptVoterId').textContent = receipt.voterId;
            document.getElementById('rcptTimestamp').textContent = receipt.timestamp;
            document.getElementById('rcptCandidateName').textContent = receipt.candidateName;
            document.getElementById('rcptCandidateParty').textContent = receipt.candidateParty;
            document.getElementById('rcptHash').textContent = receipt.hash;
        }

        function copyReceiptToClipboard() {
            const hash = document.getElementById('rcptHash').textContent.trim();
            const tempInput = document.createElement('textarea');
            tempInput.value = hash;
            document.body.appendChild(tempInput);
            tempInput.select();
            document.execCommand('copy');
            document.body.removeChild(tempInput);
            showToast('Verification hash copied to clipboard.', 'success');
        }

        function downloadReceiptFile() {
            const voterId = document.getElementById('rcptVoterId').textContent.trim();
            const ts = document.getElementById('rcptTimestamp').textContent.trim();
            const cand = document.getElementById('rcptCandidateName').textContent.trim();
            const party = document.getElementById('rcptCandidateParty').textContent.trim();
            const hash = document.getElementById('rcptHash').textContent.trim();

            const content = [
                '====================================================',
                '       CIVICPULSE DIGITAL BALLOT RECEIPT            ',
                '====================================================',
                `Voter ID        : ${voterId}`,
                `Timestamp       : ${ts}`,
                `Recorded Choice : ${cand} (${party})`,
                `Ledger Hash     : ${hash}`,
                'Status          : VERIFIED & IMMUTABLE',
                '===================================================='
            ].join('\n');

            const blob = new Blob([content], { type: 'text/plain;charset=utf-8' });
            const url = URL.createObjectURL(blob);
            const a = document.createElement('a');
            a.href = url;
            a.download = `Ballot_Receipt_${voterId}.txt`;
            document.body.appendChild(a);
            a.click();
            document.body.removeChild(a);
            URL.revokeObjectURL(url);
            showToast('Downloaded official ballot receipt.', 'info');
        }

        // =========================================================
        // ADMIN COMMAND CENTER LOGIC
        // =========================================================
        function renderAdminDashboard() {
            renderGlobalHeader();

            // Update Election Status Buttons
            const s = appState.electionStatus;
            document.getElementById('btnStatusLive').className = `px-3.5 py-2 rounded-lg text-xs font-bold transition cursor-pointer ${s === 'LIVE' ? 'bg-emerald-500 text-white shadow-xs' : 'text-slate-300 hover:text-white'}`;
            document.getElementById('btnStatusPaused').className = `px-3.5 py-2 rounded-lg text-xs font-bold transition cursor-pointer ${s === 'PAUSED' ? 'bg-amber-500 text-white shadow-xs' : 'text-slate-300 hover:text-white'}`;
            document.getElementById('btnStatusClosed').className = `px-3.5 py-2 rounded-lg text-xs font-bold transition cursor-pointer ${s === 'CLOSED' ? 'bg-rose-600 text-white shadow-xs' : 'text-slate-300 hover:text-white'}`;

            // Compute Telemetry
            const totalVotes = appState.candidates.reduce((acc, c) => acc + c.votes, 0);
            const totalVoters = appState.voters.length;
            const votedVoters = appState.voters.filter(v => v.hasVoted).length;
            const turnoutPct = totalVoters > 0 ? Math.round((votedVoters / totalVoters) * 100) : 0;

            document.getElementById('adminTotalVotesBadge').textContent = `${totalVotes} Total Vote${totalVotes === 1 ? '' : 's'}`;
            document.getElementById('lastTelemetryUpdate').textContent = `Synced at ${new Date().toLocaleTimeString([], { hour12: false })}`;

            // Leading Candidate / Tie Detection Banner
            renderLeaderBanner(totalVotes);

            // Tally Horizontal Bars
            const tallyContainer = document.getElementById('tallyBarsContainer');
            const sortedCandidates = [...appState.candidates].sort((a, b) => b.votes - a.votes);

            if (sortedCandidates.length === 0) {
                tallyContainer.innerHTML = `<p class="text-sm text-slate-500">No candidates configured.</p>`;
            } else {
                tallyContainer.innerHTML = sortedCandidates.map((cand, index) => {
                    const pct = totalVotes > 0 ? ((cand.votes / totalVotes) * 100).toFixed(1) : '0.0';
                    const isTop = index === 0 && cand.votes > 0;
                    return `
                        <div class="space-y-1.5">
                            <div class="flex items-center justify-between text-sm">
                                <div class="flex items-center gap-2">
                                    <span class="font-bold text-slate-900">${cand.name}</span>
                                    <span class="text-xs text-slate-500 hidden sm:inline">(${cand.party})</span>
                                    ${isTop ? '<span class="px-2 py-0.5 rounded-md bg-blue-50 text-cobalt-700 font-mono text-[10px] font-bold border border-blue-200">TOP</span>' : ''}
                                </div>
                                <div class="font-mono text-xs font-semibold text-slate-700">
                                    ${cand.votes} vote${cand.votes === 1 ? '' : 's'} <span class="text-slate-400">(${pct}%)</span>
                                </div>
                            </div>
                            <div class="w-full h-3 rounded-full bg-slate-100 overflow-hidden">
                                <div class="h-full rounded-full transition-all duration-500" style="width: ${pct}%; background-color: ${cand.color};"></div>
                            </div>
                        </div>
                    `;
                }).join('');
            }

            // Turnout Ring
            const circumference = 314.16; // 2 * PI * 50
            const offset = circumference - (turnoutPct / 100) * circumference;
            document.getElementById('turnoutRingCircle').style.strokeDashoffset = offset;
            document.getElementById('turnoutRingPercent').textContent = `${turnoutPct}%`;
            document.getElementById('turnoutVotedCount').textContent = votedVoters;
            document.getElementById('turnoutPendingCount').textContent = Math.max(0, totalVoters - votedVoters);

            // Admin Candidate List
            const adminCandList = document.getElementById('adminCandidateList');
            adminCandList.innerHTML = appState.candidates.map(c => `
                <div class="py-3 flex items-center justify-between gap-3">
                    <div class="flex items-center gap-3 min-w-0">
                        ${createCandidateAvatarSVG(c.name, c.color)}
                        <div class="truncate">
                            <div class="text-sm font-bold text-slate-900 truncate">${c.name}</div>
                            <div class="text-xs text-slate-500 truncate">${c.party} • <span class="font-mono">${c.votes} votes</span></div>
                        </div>
                    </div>
                    <button onclick="removeCandidate('${c.id}')" class="px-2.5 py-1.5 rounded-lg text-xs font-semibold text-rose-600 hover:bg-rose-50 border border-transparent hover:border-rose-200 transition cursor-pointer shrink-0">
                        Remove
                    </button>
                </div>
            `).join('');

            // Admin Voter List
            const adminVoterList = document.getElementById('adminVoterList');
            adminVoterList.innerHTML = appState.voters.map(v => `
                <div class="py-2.5 flex items-center justify-between gap-3 text-sm">
                    <div class="flex items-center gap-2.5 min-w-0">
                        <span class="px-2 py-0.5 rounded-md bg-slate-100 font-mono text-xs font-bold text-slate-700">${v.id}</span>
                        <span class="font-medium text-slate-800 truncate">${v.name}</span>
                    </div>
                    <span class="px-2.5 py-0.5 rounded-full text-[11px] font-semibold shrink-0 ${v.hasVoted ? 'bg-emerald-50 text-emerald-700 border border-emerald-200' : 'bg-slate-100 text-slate-500'}">
                        ${v.hasVoted ? '✓ Voted' : 'Pending'}
                    </span>
                </div>
            `).join('');

            // Audit Log Feed
            document.getElementById('auditLogCount').textContent = `${appState.auditLogs.length} Events Recorded`;
            const feed = document.getElementById('auditLogFeed');
            feed.innerHTML = appState.auditLogs.map(entry => {
                const badgeColor = entry.type === 'BALLOT'
                    ? 'text-emerald-400'
                    : entry.type === 'ADMIN'
                        ? 'text-blue-400'
                        : 'text-amber-400';
                return `
                    <div class="flex items-start gap-2.5 leading-relaxed border-b border-slate-800/80 pb-1.5 last:border-none">
                        <span class="text-slate-500 shrink-0">[${entry.time}]</span>
                        <span class="${badgeColor} font-bold shrink-0">[${entry.type}]</span>
                        <span class="text-slate-200 break-words">${entry.message}</span>
                    </div>
                `;
            }).join('');
        }

        function renderLeaderBanner(totalVotes) {
            const banner = document.getElementById('leaderBanner');
            if (appState.candidates.length === 0 || totalVotes === 0) {
                banner.className = 'rounded-2xl border border-slate-200 bg-white p-5 flex items-center justify-between gap-4 shadow-xs';
                banner.innerHTML = `
                    <div class="flex items-center gap-3">
                        <div class="w-10 h-10 rounded-xl bg-slate-100 text-slate-600 flex items-center justify-center font-bold">i</div>
                        <div>
                            <div class="text-xs font-mono uppercase text-slate-500">Standings Telemetry</div>
                            <div class="text-sm sm:text-base font-bold text-slate-900">Awaiting Ballots — No Votes Cast Yet</div>
                        </div>
                    </div>
                    <span class="text-xs font-mono text-slate-500">STATUS: ${appState.electionStatus}</span>
                `;
                return;
            }

            const maxVotes = Math.max(...appState.candidates.map(c => c.votes));
            const leaders = appState.candidates.filter(c => c.votes === maxVotes);

            if (leaders.length > 1) {
                // Tie Detected
                const names = leaders.map(l => l.name).join(' & ');
                banner.className = 'rounded-2xl border border-amber-300 bg-amber-50/70 p-5 flex flex-col sm:flex-row sm:items-center justify-between gap-4 shadow-xs';
                banner.innerHTML = `
                    <div class="flex items-center gap-3.5">
                        <div class="w-10 h-10 rounded-xl bg-amber-500 text-white flex items-center justify-center font-extrabold shrink-0">!</div>
                        <div>
                            <div class="text-xs font-mono uppercase tracking-wider text-amber-800 font-bold">Deadlock / Tie Detected</div>
                            <div class="text-base sm:text-lg font-extrabold text-slate-900">${names}</div>
                            <p class="text-xs text-amber-900 mt-0.5">Tied at <strong>${maxVotes} vote${maxVotes === 1 ? '' : 's'}</strong> each.</p>
                        </div>
                    </div>
                    <span class="px-3 py-1 rounded-full bg-amber-200/80 text-amber-900 font-mono text-xs font-bold self-start sm:self-center">TIE ALERT</span>
                `;
            } else {
                const leader = leaders[0];
                const share = ((leader.votes / totalVotes) * 100).toFixed(1);
                const label = appState.electionStatus === 'CLOSED' ? 'Certified Election Winner' : 'Current Frontrunner';
                banner.className = 'rounded-2xl border border-emerald-200 bg-emerald-50/60 p-5 flex flex-col sm:flex-row sm:items-center justify-between gap-4 shadow-xs';
                banner.innerHTML = `
                    <div class="flex items-center gap-3.5">
                        ${createCandidateAvatarSVG(leader.name, leader.color)}
                        <div>
                            <div class="text-xs font-mono uppercase tracking-wider text-emerald-700 font-bold">${label}</div>
                            <div class="text-base sm:text-lg font-extrabold text-slate-900">${leader.name} <span class="text-xs font-medium text-slate-600">(${leader.party})</span></div>
                        </div>
                    </div>
                    <div class="text-left sm:text-right">
                        <div class="text-lg font-extrabold text-emerald-700">${leader.votes} Votes (${share}%)</div>
                        <div class="text-[11px] font-mono text-slate-500">PLURALITY LEADER</div>
                    </div>
                `;
            }
        }

        function setElectionStatus(newStatus) {
            if (appState.electionStatus === newStatus) return;
            appState.electionStatus = newStatus;
            logAuditEvent('ADMIN', `Election status transitioned to ${newStatus}.`);
            saveState();
            renderAdminDashboard();
            showToast(`Election status updated to ${newStatus}.`, 'info');
        }

        function handleAddCandidate(e) {
            e.preventDefault();
            const name = document.getElementById('newCandName').value.trim();
            const party = document.getElementById('newCandParty').value.trim();
            const platform = document.getElementById('newCandPlatform').value.trim();
            if (!name || !party || !platform) return;

            const palette = ['#2563eb', '#059669', '#7c3aed', '#d97706', '#dc2626', '#0891b2'];
            const color = palette[appState.candidates.length % palette.length];
            const id = `CAND-0${appState.candidates.length + 1}-${Math.floor(10 + Math.random() * 89)}`;

            appState.candidates.push({ id, name, party, platform, color, votes: 0 });
            logAuditEvent('ADMIN', `Added candidate "${name}" (${party}) to official ballot.`);
            saveState();

            e.target.reset();
            renderAdminDashboard();
            showToast(`Added ${name} to the ballot.`, 'success');
        }

        function removeCandidate(candId) {
            if (appState.candidates.length <= 1) {
                showToast('At least one candidate must remain on the ballot.', 'warning');
                return;
            }
            const cand = appState.candidates.find(c => c.id === candId);
            appState.candidates = appState.candidates.filter(c => c.id !== candId);
            logAuditEvent('ADMIN', `Removed candidate "${cand ? cand.name : candId}" from ballot.`);
            saveState();
            renderAdminDashboard();
            showToast('Candidate removed from ballot.', 'info');
        }

        function handleAddVoter(e) {
            e.preventDefault();
            const rawId = document.getElementById('newVoterId').value.trim().toUpperCase();
            const name = document.getElementById('newVoterName').value.trim();
            if (!rawId || !name) return;

            if (appState.voters.some(v => v.id.toUpperCase() === rawId)) {
                showToast(`Voter ID ${rawId} is already registered.`, 'error');
                return;
            }

            appState.voters.push({ id: rawId, name, hasVoted: false, receipt: null });
            logAuditEvent('ADMIN', `Registered new eligible Voter ID ${rawId} (${name}).`);
            saveState();

            e.target.reset();
            renderAdminDashboard();
            showToast(`Registered Voter ID ${rawId}.`, 'success');
        }

        // =========================================================
        // DEMO RESET WORKFLOW
        // =========================================================
        function openResetModal() {
            document.getElementById('resetDemoModal').classList.remove('hidden');
        }

        function closeResetModal() {
            document.getElementById('resetDemoModal').classList.add('hidden');
        }

        function executeDemoReset() {
            appState = getDefaultState();
            saveState();
            clearVoterSession();
            closeResetModal();
            if (activeRole === 'admin') {
                renderAdminDashboard();
            } else {
                renderVoterPortal();
            }
            showToast('All election demo data has been reset to defaults.', 'success');
        }

        // Initialize on Page Load
        window.addEventListener('DOMContentLoaded', () => {
            switchRole('voter');
        });
    </script>
</body>
</html>
```
